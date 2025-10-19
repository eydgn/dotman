#include "utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "core.h"
#include "log.h"

int find_by_name(const char* name, entry_t* entries) {
    int i = 0;
    for (; (size_t) i < entries->len; i++) {
        if (strcmp(entries->data[i].entry->str[0], name) == 0) {
            return i;
            break;
        }
    }
    return -1;
}

char getch(void) {
    struct termios oldt;
    struct termios newt;
    char           ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = (char) getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int edit_save(char* name, char* source, char* target, int index, entry_t* entries) {
    if (svec_set(entries->data[index].entry, 0, name) == EXIT_FAILURE) {
        LOG_ERROR("Failed to save name.");
        return EXIT_FAILURE;
    }
    if (svec_set(entries->data[index].entry, 1, source) == EXIT_FAILURE) {
        LOG_ERROR("Failed to save source.");
        return EXIT_FAILURE;
    }
    if (svec_set(entries->data[index].entry, 2, target) == EXIT_FAILURE) {
        LOG_ERROR("Failed to save target.");
        return EXIT_FAILURE;
    }
    printf("Changes saved!\n");
    return EXIT_SUCCESS;
}

int user_confirm(const char* msg) {
    if (!msg) {
        LOG_ERROR("Message is NULL");
        return EXIT_FAILURE;
    }
    printf("%s (y/n): ", msg);
    char response = getch();
    printf("%c\n", response);

    if (tolower(response) == 'y') {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

char* expand_home(const char* path) {
    if (!path) {
        LOG_ERROR("path is NULL");
        return NULL;
    }

    if (path[0] != '~') {
        return strdup(path);
    }

    const char* home = getenv("HOME");
    if (!home) {
        LOG_ERROR("Failed to get $HOME");
        return NULL;
    }

    size_t size      = strlen(home) + strlen(path) + 1;
    char*  full_path = malloc(size);
    if (!full_path) {
        LOG_ERROR("malloc failed");
        return NULL;
    }

    int ret = snprintf(full_path, size, "%s%s", home, path + 1);
    if (ret < 0 || (size_t) ret >= size) {
        LOG_ERROR("snprintf failed");
        free(full_path);
        return NULL;
    }

    return full_path;
}

int check_link(const char* source, const char* target) {
    if (!source || !target) {
        LOG_ERROR("source or target is NULL");
        return EXIT_FAILURE;
    }

    char* src = expand_home(source);
    char* trg = expand_home(target);
    if (!src || !trg) {
        LOG_ERROR("expand_home failed");
        free(src);
        free(trg);
        return EXIT_FAILURE;
    }

    bool src_exists = (access(src, F_OK) == 0);
    bool trg_exists = (access(trg, F_OK) == 0);

    if (trg_exists) {
        struct stat st;
        if (lstat(trg, &st) == 0 && S_ISLNK(st.st_mode)) {
            LOG_ERROR("Target is a symlink");
            free(src);
            free(trg);
            return EXIT_FAILURE;
        }
    }

    if (src_exists && trg_exists) {
        LOG_ERROR("Target and source exist.");
        free(src);
        free(trg);
        return EXIT_FAILURE;
    }

    if (src_exists && !trg_exists) {
        if (symlink(src, trg) == 0) {
            LOG_INFO("Symbolic link created.\nFrom: %s\tTo: %s", src, trg);
            free(src);
            free(trg);
            return EXIT_SUCCESS;
        }
        LOG_ERROR("Failed to create symbolic link.");
        free(src);
        free(trg);
        return EXIT_FAILURE;
    }

    if (trg_exists && !src_exists) {
        if (user_confirm("Move target to source and create symlink?") == EXIT_SUCCESS) {
            if (rename(trg, src) != 0) {
                LOG_ERROR("Rename failed");
                free(src);
                free(trg);
                return EXIT_FAILURE;
            }
            if (symlink(src, trg) == 0) {
                LOG_INFO("Symbolic link created.\nFrom: %s\tTo: %s", src, trg);
                free(src);
                free(trg);
                return EXIT_SUCCESS;
            }
            LOG_ERROR("Failed to create symbolic link.");
            free(src);
            free(trg);
            return EXIT_FAILURE;
        }
        LOG_INFO("Operation canceled by user.");
        free(src);
        free(trg);
        return EXIT_FAILURE;
    }

    LOG_ERROR("There are no target and source");
    free(src);
    free(trg);
    return EXIT_FAILURE;
}
