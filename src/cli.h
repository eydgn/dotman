#ifndef CLI_H
#define CLI_H

#include "core.h"

typedef enum {
    CMD_ADD,
    CMD_DEL,
    CMD_LIST,
    CMD_EDIT,
    CMD_SYNC,
    CMD_INIT,
    CMD_BACKUP,
    CMD_HELP,
    CMD_VER,
    CMD_ERROR,
} cli_action_t;

typedef struct {
    cli_action_t action;
    svec_t       args;
} cmd_t;

int extract_action(cmd_t* cmd, const char* action);
int copy_args(cmd_t* cmd, int argc, char* argv[]);

int         find_by_name(const char* name, entry_t* entries);
static char getch(void);
int         edit_save(char* name, char* source, char* target, int index, entry_t* entries);
int         check_link(const char* source, const char* target);
int         user_confirm(const char* msg);
char*       expand_home(const char* path);

int cmd_add(cmd_t* cmd, entry_t* entries);
int cmd_del(cmd_t* cmd, entry_t* entries);
int cmd_list(entry_t* entries);
int cmd_edit(cmd_t* cmd, entry_t* entries);
int cmd_sync(cmd_t* cmd, entry_t* entries);
int cmd_init(cmd_t* cmd, entry_t* entries);
int cmd_backup(cmd_t* cmd, entry_t* entries);
int cmd_help(cmd_t* cmd);
int cmd_version(cmd_t* cmd);
int cmd_error(void);

int exec_cmd(cmd_t* cmd, entry_t* entries);
#endif  // !CLI_H
