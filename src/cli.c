#include "cli.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

int cmd_add(cmd_t* cmd, entry_t* entries) {
  if (!cmd || cmd->args.len < 3 || !entries) {
    LOG_ERROR("cmd or entries is NULL, or there are fewer than 3 arguments.");
    return EXIT_FAILURE;
  }

  // check source
  if (access(cmd->args.str[1], F_OK)) {
    LOG_ERROR("The source file or directory does not exist.");
    return EXIT_FAILURE;
  }
  // check target
  if (access(cmd->args.str[2], F_OK) == 0) {
    LOG_ERROR("The target file or directory already exists.");
    LOG_INFO("Please copy it to the dotfile home directory and try again.");
    return EXIT_FAILURE;
  }
  // check if it's a symlink
  struct stat st;
  if (lstat(cmd->args.str[2], &st) == 0) {
    if (S_ISLNK(st.st_mode)) {
      LOG_ERROR("A symbolic link already exists at the target location.");
      return EXIT_FAILURE;
    }
  }

  // create the symlink
  if (symlink(cmd->args.str[1], cmd->args.str[2]) == 0) {
    LOG_INFO("Symbolic link created.\nFrom: %s\tTo: %s", cmd->args.str[1],
             cmd->args.str[2]);
  } else {
    LOG_ERROR("Failed to create symbolic link.");
    return EXIT_FAILURE;
  }

  // push data
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

  svec_free(&tmp.entry);
  return EXIT_SUCCESS;
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
