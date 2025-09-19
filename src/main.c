#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"
#define BUFFER 1024

int main(void) {
  printf("Testing write_config function...\n");

  // Create a config_line_vec_t
  config_line_vec_t* config_vec = NULL;
  if (config_line_vec_new(&config_vec) != 0) {
    LOG_ERROR("Failed to create config vector");
    return EXIT_FAILURE;
  }

  // Create test config lines

  // Test line 1: dot scope with fields and array
  config_line_t line1;
  line1.scope = SCOPE_DOT;
  if (str_vec_new(&line1.fields) != 0 || str_vec_new(&line1.array) != 0) {
    LOG_ERROR("Failed to create vectors for line1");
    return EXIT_FAILURE;
  }
  str_vec_push(line1.fields, "file1.txt");
  str_vec_push(line1.fields, "/home/user/config");
  str_vec_push(line1.array, "backup");
  str_vec_push(line1.array, "sync");
  config_line_vec_push(config_vec, line1);

  // Test line 2: dep scope with fields only
  config_line_t line2;
  line2.scope = SCOPE_DEP;
  if (str_vec_new(&line2.fields) != 0 || str_vec_new(&line2.array) != 0) {
    LOG_ERROR("Failed to create vectors for line2");
    return EXIT_FAILURE;
  }
  str_vec_push(line2.fields, "package1");
  str_vec_push(line2.fields, "version1.0");
  config_line_vec_push(config_vec, line2);

  // Test line 3: another dot scope
  config_line_t line3;
  line3.scope = SCOPE_DOT;
  if (str_vec_new(&line3.fields) != 0 || str_vec_new(&line3.array) != 0) {
    LOG_ERROR("Failed to create vectors for line3");
    return EXIT_FAILURE;
  }
  str_vec_push(line3.fields, "another_file.conf");
  str_vec_push(line3.fields, "/etc/config");
  str_vec_push(line3.array, "important");
  config_line_vec_push(config_vec, line3);

  // Test line 4: another dep scope
  config_line_t line4;
  line4.scope = SCOPE_DEP;
  if (str_vec_new(&line4.fields) != 0 || str_vec_new(&line4.array) != 0) {
    LOG_ERROR("Failed to create vectors for line4");
    return EXIT_FAILURE;
  }
  str_vec_push(line4.fields, "anotherpackage");
  str_vec_push(line4.fields, "version2.0");
  config_line_vec_push(config_vec, line4);

  // Print original order
  printf("\nOriginal config lines:\n");
  for (size_t i = 0; i < config_vec->len; i++) {
    printf("Line %zu: scope=%s, fields=%zu, array=%zu\n", i,
           scope_enum_to_str(config_vec->data[i].scope),
           config_vec->data[i].fields->len, config_vec->data[i].array->len);
  }

  // Test write_config
  const char* test_filename = "test_output.conf";
  printf("\nWriting config to file: %s\n", test_filename);

  if (write_config(config_vec, test_filename) == EXIT_SUCCESS) {
    printf("✓ write_config succeeded!\n");

    // Read and display the written file
    FILE* file = fopen(test_filename, "r");
    if (file) {
      printf("\nGenerated file content:\n");
      printf("------------------------\n");
      char buffer[BUFFER];
      while (fgets(buffer, (int) sizeof(buffer), file)) {
        printf("%s", buffer);
      }
      printf("------------------------\n");
      fclose(file);
    }
  } else {
    printf("✗ write_config failed!\n");
  }

  // Cleanup
  for (size_t i = 0; i < config_vec->len; i++) {
    str_vec_free(&config_vec->data[i].fields);
    str_vec_free(&config_vec->data[i].array);
  }
  config_line_vec_free(&config_vec);

  return EXIT_SUCCESS;
}
