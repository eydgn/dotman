#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

cmd_type_t extract_cmd(const char* cmd) {
  if (!(strcmp("add", cmd))) {
    return CMD_ADD;
  }
  if (!(strcmp("del", cmd))) {
    return CMD_DEL;
  }
  if (!(strcmp("list", cmd))) {
    return CMD_LIST;
  }
  if (!(strcmp("edit", cmd))) {
    return CMD_EDIT;
  }
  if (!(strcmp("sync", cmd))) {
    return CMD_SYNC;
  }
  if (!(strcmp("init", cmd))) {
    return CMD_INIT;
  }
  if (!(strcmp("backup", cmd))) {
    return CMD_BACKUP;
  }
  if (!(strcmp("help", cmd))) {
    return CMD_HELP;
  }
  if (!(strcmp("ver", cmd))) {
    return CMD_VERSION;
  }
  return CMD_UNKNOWN;
}

scope_type_t extract_scope(const char* scp) {
  if (!(strcmp("dot", scp))) {
    return SCOPE_DOT;
  }
  if (!(strcmp("dep", scp))) {
    return SCOPE_DEP;
  }
  return SCOPE_UNKNOWN;
}

int extract_array_cli_cmd(cli_cmd_type_t* cli_cmd, char* data) {
  EXTRACT_ARRAY(cli_cmd, data);
  return EXIT_SUCCESS;
}

int parse_args(int argc, char** argv, cli_cmd_type_t* result) {
  if (!result) {
    return false;
  }
  result->cmd    = CMD_NONE;
  result->scope  = SCOPE_NONE;
  result->fields = NULL;
  result->array  = NULL;

  for (int i = 1; i < argc; i++) {
    if (i == 1) {
      result->cmd = extract_cmd(argv[i]);
    } else if (i == 2) {
      result->scope = extract_scope(argv[i]);
    } else {
      if (str_vec_new(&result->fields) != 0) {
        LOG_ERROR("Failed to create str_vec for result->fields");
        return EXIT_FAILURE;
      }
      if (str_vec_new(&result->array) != 0) {
        LOG_ERROR("Failed to create str_vec for result->array");
        str_vec_free(&result->fields);
        return EXIT_FAILURE;
      }

      if ((strchr(argv[i], ',')) != NULL) {
        if (extract_array_cli_cmd(result, argv[i])) {
          LOG_ERROR("Failed to extract array");
          str_vec_free(&result->fields);
          str_vec_free(&result->array);
          return EXIT_FAILURE;
        }
        continue;
      }
      str_vec_push(result->fields, argv[i]);
    }
  }
  return EXIT_SUCCESS;
}
