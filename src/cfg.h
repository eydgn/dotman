#ifndef CFG_H
#define CFG_H

#include "core.h"
// name,target,source;

int parse_line(entry_t* entries, char* line);
int read_cfg(const char* filename, entry_t** entries);
int write_cfg(entry_t* entries, const char* filename);

#endif  // !CFG_H
