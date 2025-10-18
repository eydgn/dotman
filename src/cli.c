#include "cli.h"

#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "log.h"

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

int exec_cmd(cmd_t* cmd) {
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
      return cmd_list(cmd, entries);
    case CMD_EDIT:
      return cmd_edit(cmd, entries);
    case CMD_SYNC:
      return cmd_sync(cmd, entries);
    case CMD_INIT:
      return cmd_init(cmd, entries);
    case CMD_BACKUP:
      return cmd_backup(cmd, entries);
    case CMD_HELP:
      return cmd_help(cmd );
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
