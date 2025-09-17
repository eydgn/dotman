#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#include "common.h"
#include "cvector.h"

typedef struct {
  scope_type_t scope;
  str_vec_t*   fields;
  str_vec_t*   array;
} config_line_t;

DEFINE_VECTOR(config_line_t, config_line)

char* trim(char* str);
int   extract_array(config_line_t* config_line, char* data);
int   parse_line(config_line_vec_t* config_line_vec, char* line);
int   parse_file(const char* filename, config_line_vec_t** config_line_vec);
int   sort_by_enums(config_line_vec_t* config_line_vec);

#endif
