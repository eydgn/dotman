#ifndef CLI_H
#define CLI_H

#include <stdbool.h>
#include <stddef.h>

#include "common.h"
#include "config.h"

#define DOTMAN_VERSION "v0.1.0"
typedef enum {
  CMD_NONE = 0,
  CMD_ADD,
  CMD_DEL,
  CMD_LIST,
  CMD_EDIT,
  CMD_SYNC,
  CMD_INIT,
  CMD_BACKUP,
  CMD_HELP,
  CMD_VERSION,
  CMD_UNKNOWN,
} cmd_type_t;

typedef struct {
  cmd_type_t   cmd;
  scope_type_t scope;
  str_vec_t*   fields;
  str_vec_t*   array;
} cli_cmd_type_t;

cmd_type_t   extract_cmd(const char* cmd);
scope_type_t extract_scope(const char* scp);
int          extract_array_cli_cmd(cli_cmd_type_t* cli_cmd, char* data);
int          parse_args(int argc, char** argv, cli_cmd_type_t* result);

int cmd_add(cli_cmd_type_t* cmd, config_line_vec_t* vec);
int cmd_del(cli_cmd_type_t* cmd);
int cmd_list(cli_cmd_type_t* cmd);
int cmd_edit(cli_cmd_type_t* cmd);
int cmd_sync(cli_cmd_type_t* cmd);
int cmd_init(cli_cmd_type_t* cmd);
int cmd_backup(cli_cmd_type_t* cmd);
int cmd_help(cli_cmd_type_t* cmd);
int cmd_version(cli_cmd_type_t* cmd);
int cmd_unknown(void);

int execute_cmd(cli_cmd_type_t* cmd, config_line_vec_t* vec);

#endif  // !CLI_H
