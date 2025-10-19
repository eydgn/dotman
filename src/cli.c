#include "cli.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "core.h"
#include "log.h"
#include "utils.h"

int extract_action(cmd_t* cmd, const char* action) {
    if (!cmd || !action) {
        LOG_ERROR("cmd or action is NULL.");
        return EXIT_FAILURE;
    }

    if (!(strcmp("add", action))) {
        cmd->action = CMD_ADD;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("del", action))) {
        cmd->action = CMD_DEL;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("list", action))) {
        cmd->action = CMD_LIST;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("edit", action))) {
        cmd->action = CMD_EDIT;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("sync", action))) {
        cmd->action = CMD_SYNC;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("init", action))) {
        cmd->action = CMD_INIT;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("backup", action))) {
        cmd->action = CMD_BACKUP;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("help", action))) {
        cmd->action = CMD_HELP;
        return EXIT_SUCCESS;
    }

    if (!(strcmp("ver", action))) {
        cmd->action = CMD_VER;
        return EXIT_SUCCESS;
    }

    cmd->action = CMD_ERROR;
    return EXIT_SUCCESS;
}

int copy_args(cmd_t* cmd, int argc, char* argv[]) {
    if (!cmd || argc < 1 || !argv) {
        LOG_ERROR("cmd or argv is NULL, or argc is less than 1.");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < argc; i++) {
        if (svec_push(&cmd->args, argv[i])) {
            LOG_ERROR("Failed to add argument to vector.");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int cmd_add(cmd_t* cmd, entry_t* entries) {
    if (!cmd || cmd->args.len < 3 || !entries) {
        LOG_ERROR("cmd or entries is NULL, or there are fewer than 3 arguments.");
        return EXIT_FAILURE;
    }

    entry_ref_t tmp;
    svec_new(&tmp.entry);
    for (size_t i = 0; i < cmd->args.len; i++) {
        if (svec_push(tmp.entry, cmd->args.str[i])) {
            LOG_ERROR("Failed to add argument to entry vector.");
            svec_free(&tmp.entry);
            return EXIT_FAILURE;
        }
    }

    if (entry_push(entries, tmp)) {
        LOG_ERROR("Failed to push to entries");
        svec_free(&tmp.entry);
        return EXIT_FAILURE;
    }

    LOG_INFO("entry is added to config file.");
    svec_free(&tmp.entry);
    return EXIT_SUCCESS;
}

int cmd_del(cmd_t* cmd, entry_t* entries) {
    // Destroy the link if exists
    // remove from the entries
    if (!cmd || cmd->args.len != 1 || !entries) {
        LOG_ERROR("cmd or entries is NULL, or there are more or less than 1 arguments.");
        return EXIT_FAILURE;
    }

    int index = find_by_name(cmd->args.str[0], entries);
    if (index == -1) {
        LOG_ERROR("Given dotfile not found in the cfg.");
        return EXIT_FAILURE;
    }

    struct stat st;
    if (lstat(entries->data->entry->str[2], &st) == 0) {
        if (S_ISLNK(st.st_mode)) {
            if (unlink(entries->data[index].entry->str[2]) == 0) {
                LOG_INFO("Symbolic link is destroyed.");
            } else {
                LOG_ERROR("Failed to destroy symbolic link.");
                return EXIT_FAILURE;
            }
        } else {
            LOG_WARN("There is no symbolic link.");
        }
    }

    entry_ref_t tmp;
    svec_new(&tmp.entry);
    entry_del(entries, (size_t) index, &tmp);
    if (!tmp.entry) {
        LOG_ERROR("Failed to delete entry");
        return EXIT_FAILURE;
    }

    LOG_INFO("\"%s\" removed from config file.", tmp.entry->str[1]);
    svec_free(&tmp.entry);
    return EXIT_SUCCESS;
}

int cmd_list(entry_t* entries) {
    if (!entries) {
        LOG_ERROR("entries is NULL.");
        return EXIT_FAILURE;
    }

    printf("%-20s %-40s %-40s %-10s\n", "Name", "Source", "Target", "Symlink");

    for (size_t i = 0; i < entries->len; i++) {
        struct stat st;
        int is_link = (lstat(entries->data[i].entry->str[2], &st) == 0) && S_ISLNK(st.st_mode);
        printf(
            "%-20s %-40s %-40s %s%-10s%s\n",
            entries->data[i].entry->str[0],
            entries->data[i].entry->str[1],
            entries->data[i].entry->str[2],
            is_link ? COLOR_GREEN : COLOR_RED,
            is_link ? "yes" : "no",
            COLOR_RESET);
    }

    return EXIT_SUCCESS;
}

int cmd_edit(cmd_t* cmd, entry_t* entries) {
    int index = find_by_name(cmd->args.str[0], entries);
    if (index == -1) {
        LOG_ERROR("Given dotfile not found in the cfg.");
        return EXIT_FAILURE;
    }

    char name[128];
    char source[128];
    char target[128];

    strncpy(name, entries->data[index].entry->str[0], sizeof(name));
    strncpy(source, entries->data[index].entry->str[1], sizeof(source));
    strncpy(target, entries->data[index].entry->str[2], sizeof(target));

    bool running = true;
    bool saved   = false;

    while (running) {
        printf("Current Values\n");
        printf("1. %s\n", name);
        printf("2. %s\n", source);
        printf("3. %s\n", target);
        printf("Enter number to edit, w to save, q to discard\n");
        char c = getch();

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            ;
        }

        if (c == '1') {
            printf("Enter new value for 1: ");
            if (fgets(name, sizeof(name), stdin)) {
                name[strcspn(name, "\n")] = 0;
            }
        } else if (c == '2') {
            printf("Enter new value for 2: ");
            if (fgets(source, sizeof(source), stdin)) {
                source[strcspn(source, "\n")] = 0;
            }
        } else if (c == '3') {
            printf("Enter new value for 3: ");
            if (fgets(target, sizeof(target), stdin)) {
                target[strcspn(target, "\n")] = 0;
            }
        } else if (c == 'w') {
            saved   = true;
            running = false;
        } else if (c == 'q') {
            running = false;
        } else {
            printf("Invalid input.\n");
        }
    }

    if (saved) {
        if (edit_save(name, source, target, index, entries)) {
            LOG_ERROR("Failed to save changes.");
            return EXIT_FAILURE;
        }
        LOG_INFO("Run sync command to create links.");
    } else {
        printf("Changes discarded.\n");
    }

    return EXIT_SUCCESS;
}

int exec_cmd(cmd_t* cmd, entry_t* entries) {
    if (!cmd) {
        LOG_ERROR("cmd is NULL.");
        return EXIT_FAILURE;
    }

    switch (cmd->action) {
        case CMD_ADD:
            return cmd_add(cmd, entries);
        case CMD_DEL:
            return cmd_del(cmd, entries);
        case CMD_LIST:
            return cmd_list(entries);
        case CMD_EDIT:
            return cmd_edit(cmd, entries);
        case CMD_SYNC:
            return cmd_sync(cmd, entries);
        case CMD_INIT:
            return cmd_init(cmd, entries);
        case CMD_BACKUP:
            return cmd_backup(cmd, entries);
        case CMD_HELP:
            return cmd_help(cmd);
        case CMD_VER:
            return cmd_version(cmd);
        case CMD_ERROR:
            return cmd_error();
        default:
            LOG_ERROR("Unexpected command action type.");
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
