#include "cfg.h"

#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "log.h"

/*
 * create svec_t
 * read data to svec_t
 * push it to entry
 *
 */

int parse_line(entry_t* entries, char* line) {
  if (!entries || !line) {
    LOG_ERROR("entries or line is NULL");
    return EXIT_FAILURE;
  }
  svec_t* entry;
  svec_new(&entry);

  char* saveptr;
  char* token = strtok_r(line, ",", &saveptr);

  while (token != NULL) {
    if (*token != '\0') {
      svec_push(entry, token);
      token = strtok_r(line, ",", &saveptr);
    }
  }

  entry_push(entries, *(entry));
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
  if (fseek(file_ptr, (size_t) 0, SEEK_END)) {
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

  if (fseek(file_ptr, (size_t) 0, SEEK_SET)) {
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
  char* token = strtok_r(buffer, ";", &saveptr);

  while (token != NULL) {
    if (*token != '\0') {
      if (parse_line((*entries), token)) {
        LOG_ERROR("Failed to parse line");
        free(buffer);
        return EXIT_FAILURE;
      }

      token = strtok_r(NULL, ";", &saveptr);
    }
  }

  free(buffer);

  return EXIT_SUCCESS;
}
