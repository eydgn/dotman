#include "cfg.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "log.h"

/*
 * create svec_t
 * read data to svec_t
 * push it to entry
 *
 */

int parse_line(svec_t** entry, char* line) {
    if (!entry || !line) {
        LOG_ERROR("entries or line is NULL");
        return EXIT_FAILURE;
    }

    svec_new(entry);

    size_t field_c = 0;
    char*  saveptr;
    char*  token = strtok_r(line, ",", &saveptr);

    while (token != NULL) {
        if (*token == '\0') {
            LOG_ERROR("entry has empty fields");
            svec_free(entry);
            return EXIT_FAILURE;
        }

        if (svec_push(*entry, token) == EXIT_FAILURE) {
            LOG_ERROR("svec_push failed");
            svec_free(entry);
            return EXIT_FAILURE;
        }

        token = strtok_r(NULL, ",", &saveptr);
        field_c++;
    }

    if (field_c != 3) {
        LOG_ERROR("entry has more or less than 3 fields");
        svec_free(entry);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int read_cfg(const char* filename, entry_t** entries) {
    if (!filename || !entries) {
        LOG_ERROR("filename or entries are NULL");
        return EXIT_FAILURE;
    }

    // open file
    FILE* file_ptr = fopen(filename, "rb");
    if (!file_ptr) {
        LOG_ERROR("Failed to open file");
        return EXIT_FAILURE;
    }

    // get file size
    if (fseek(file_ptr, 0, SEEK_END) == EXIT_FAILURE) {
        LOG_ERROR("fseek failed");
        (void) fclose(file_ptr);
        return EXIT_FAILURE;
    }

    long file_size_long = ftell(file_ptr);
    if (file_size_long < 0) {
        LOG_ERROR("ftell failed");
        (void) fclose(file_ptr);
        return EXIT_FAILURE;
    }

    size_t file_size = (size_t) file_size_long;

    if (fseek(file_ptr, 0, SEEK_SET) == EXIT_FAILURE) {
        LOG_ERROR("fseek failed");
        (void) fclose(file_ptr);
        return EXIT_FAILURE;
    }

    // read file to buffer and close the file
    char* buffer = malloc(file_size + 1);

    if (!buffer) {
        LOG_ERROR("Failed to allocate buffer");
        (void) fclose(file_ptr);
        return EXIT_FAILURE;
    }

    size_t read  = fread(buffer, (size_t) 1, file_size, file_ptr);
    buffer[read] = '\0';
    (void) fclose(file_ptr);

    if (read != file_size) {
        LOG_ERROR("Failed to read file");
        free(buffer);
        return EXIT_FAILURE;
    }

    char* saveptr;
    char* token = strtok_r(buffer, "\n", &saveptr);

    while (token != NULL) {
        if (*token != '\0') {
            svec_t* result;
            if (parse_line(&result, token) == EXIT_FAILURE) {
                LOG_ERROR("Failed to parse line");
                entry_free(entries);
                free(buffer);
                return EXIT_FAILURE;
            }
            entry_push(*entries, (entry_ref_t) {.entry = result});
        }
        token = strtok_r(NULL, "\n;", &saveptr);
    }

    free(buffer);

    return EXIT_SUCCESS;
}

int sort_by_names(entry_t* entries) {
    if (!entries || entries->len == 0) {
        LOG_ERROR("entries is empty");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < entries->len; i++) {
        if (entries->data[i].entry->len != 3) {
            LOG_ERROR("entries has less or more than 3 fileds");
            return EXIT_FAILURE;
        }
        for (size_t j = 0; j < entries->len - i - 1; j++) {
            if (strcmp(entries->data[j].entry->str[0], entries->data[j + 1].entry->str[0]) > 0) {
                entry_ref_t tmp      = entries->data[j];
                entries->data[j]     = entries->data[j + 1];
                entries->data[j + 1] = tmp;
            }
        }
    }

    return EXIT_SUCCESS;
}

int write_cfg(entry_t* entries, const char* filename) {
    if (!filename || !entries) {
        LOG_ERROR("filename or entries are NULL");
        return EXIT_FAILURE;
    }

    if (sort_by_names(entries) == EXIT_FAILURE) {
        LOG_ERROR("sort_by_names failed");
        return EXIT_FAILURE;
    }

    FILE* file_ptr = fopen(filename, "w");
    if (!file_ptr) {
        LOG_ERROR("Failed to open file for writing");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < entries->len; i++) {
        if (fprintf(
                file_ptr,
                "%s,%s,%s\n",
                entries->data[i].entry->str[0],
                entries->data[i].entry->str[1],
                entries->data[i].entry->str[2])
            < 0) {
            LOG_ERROR("Failed to write file");
            (void) fclose(file_ptr);
            return EXIT_FAILURE;
        }
    }

    (void) fclose(file_ptr);
    return EXIT_SUCCESS;
}
