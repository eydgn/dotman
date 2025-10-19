#include "core.h"

int   find_by_name(const char* name, entry_t* entries);
char  getch(void);
int   edit_save(char* name, char* source, char* target, int index, entry_t* entries);
int   check_link(const char* source, const char* target);
int   user_confirm(const char* msg);
char* expand_home(const char* path);
