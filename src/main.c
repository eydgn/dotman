#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"

int main(void) {
  const char*        filename        = "test_conf.scp";
  config_line_vec_t* config_line_vec = NULL;
  parse_file(filename, &config_line_vec);

  for (size_t i = 0; i < config_line_vec->len; i++) {
    printf("SCOPE: %s\n", scope_enum_to_str(config_line_vec->data[i].scope));

    printf("Fields:\n");
    for (size_t j = 0; j < config_line_vec->data[i].fields->len; j++) {
      printf("\t%s\n", config_line_vec->data[i].fields->str[j]);
    }

    printf("Array:\n");
    for (size_t j = 0; j < config_line_vec->data[i].array->len; j++) {
      printf("\t%s\n", config_line_vec->data[i].array->str[j]);
    }
  }

  for (size_t i = 0; i < config_line_vec->len; i++) {
    str_vec_free(&config_line_vec->data[i].fields);
    str_vec_free(&config_line_vec->data[i].array);
  }
  config_line_vec_free(&config_line_vec);

  return 0;
}
