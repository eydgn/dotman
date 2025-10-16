#ifndef CFG_H
#define CFG_H

#include "core.h"
// name,target,source;

int parse_line(svec_t** entry, char* line);
int read_cfg(const char* filename, entry_t** entries);
int write_cfg(entry_t* entries, const char* filename);
int sort_by_names(entry_t* entries);

#endif  // !CFG_H
