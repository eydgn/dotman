#include "cfg.h"

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
    if (*token == '\0') {
      LOG_ERROR("Empty field in the config file!");
      svec_free(&entry);
      return EXIT_FAILURE;
    }

    svec_push(entry, token);
    token = strtok_r(line, ",", &saveptr);
  }

  entry_push(entries, *(entry));
  return EXIT_SUCCESS;
}
