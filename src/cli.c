#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"

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
  if (!(strcmp("pkg", scp))) {
    return SCOPE_PKG;
  }
  if (!(strcmp("bld", scp))) {
    return SCOPE_BLD;
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

int cmd_add(cli_cmd_type_t* cmd, config_line_vec_t* vec) {
  if (cmd->scope == SCOPE_NONE) {
    LOG_ERROR("Use with a scope.");
    printf("Check help.\n");
    return EXIT_FAILURE;
  }

  if (cmd->scope == SCOPE_SET || cmd->scope == SCOPE_UNKNOWN) {
    LOG_ERROR("Wrong use.");
    printf("Check help.\n");
    return EXIT_FAILURE;
  }

  config_line_t line;
  line.scope  = cmd->scope;
  line.fields = cmd->fields;
  line.array  = cmd->array;
  config_line_vec_push(vec, line);
  return EXIT_SUCCESS;
}

int cmd_del(cli_cmd_type_t* cmd) {
  if (cmd->scope == SCOPE_NONE) {
    LOG_ERROR("Use with a scope");
    return EXIT_FAILURE;
  }
  printf("cmd_delete function executed. Scope: %d\n", cmd->scope);
  return EXIT_SUCCESS;
}

int cmd_list(cli_cmd_type_t* cmd) {
  if (cmd->scope == SCOPE_NONE) {
    LOG_ERROR("Use with a scope");
    return EXIT_FAILURE;
  }
  printf("cmd_list function executed. Scope: %d\n", cmd->scope);
  return EXIT_SUCCESS;
}

int cmd_edit(cli_cmd_type_t* cmd) {
  if (cmd->scope == SCOPE_NONE) {
    LOG_ERROR("Use with a scope");
    return EXIT_FAILURE;
  }
  printf("cmd_edit function executed. Scope: %d\n", cmd->scope);
  return EXIT_SUCCESS;
}

int cmd_sync(cli_cmd_type_t* cmd) {
  if (cmd->scope != SCOPE_NONE) {
    LOG_ERROR("Use without scope.\n");
    return EXIT_FAILURE;
  }
  printf("cmd_sync function executed\n");
  return EXIT_SUCCESS;
}

int cmd_init(cli_cmd_type_t* cmd) {
  if (cmd->scope != SCOPE_NONE) {
    LOG_ERROR("Use without scope.\n");
    return EXIT_FAILURE;
  }
  printf("cmd_init function executed\n");
  return EXIT_SUCCESS;
}

int cmd_backup(cli_cmd_type_t* cmd) {
  if (cmd->scope != SCOPE_NONE) {
    LOG_ERROR("Use without scope.\n");
    return EXIT_FAILURE;
  }
  printf("cmd_backup function executed\n");
  return EXIT_SUCCESS;
}

int cmd_help(cli_cmd_type_t* cmd) {
  if (cmd->scope != SCOPE_NONE) {
    LOG_ERROR("Help per-scope not implemented\n");
    return EXIT_FAILURE;
  }
  printf("cmd_help function executed\n");
  return EXIT_SUCCESS;
}

int cmd_version(cli_cmd_type_t* cmd) {
  if (cmd->scope != SCOPE_NONE) {
    LOG_ERROR("Use without scope.\n");
    return EXIT_FAILURE;
  }
  printf("cmd_version function executed\n");
  return EXIT_SUCCESS;
}

int cmd_unknown(void) {
  LOG_ERROR("Unknown command.\n");
  LOG_INFO("Check help.\n");
  return EXIT_FAILURE;
}

int execute_cmd(cli_cmd_type_t* cmd, config_line_vec_t* vec) {
  if (!cmd) {
    LOG_ERROR("No command struct provided.\n");
    return EXIT_FAILURE;
  }

  switch (cmd->cmd) {
    case CMD_ADD:
      return cmd_add(cmd, vec);
    case CMD_DEL:
      return cmd_del(cmd, vec);
    case CMD_LIST:
      return cmd_list(cmd, vec);
    case CMD_EDIT:
      return cmd_edit(cmd, vec);
    case CMD_SYNC:
      return cmd_sync(cmd);
    case CMD_INIT:
      return cmd_init(cmd);
    case CMD_BACKUP:
      return cmd_version(cmd);
    case CMD_HELP:
      return cmd_help(cmd);
    case CMD_VERSION:
      return cmd_backup(cmd);
    case CMD_UNKNOWN:
      return cmd_unknown();
    case CMD_NONE:
      break;
  }
  return EXIT_SUCCESS;
}
