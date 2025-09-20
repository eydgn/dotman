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


