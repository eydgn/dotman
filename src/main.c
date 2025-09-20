#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "common.h"

int main(int argc, char* argv[]) {
  cli_cmd_type_t cmd;

  // Parse command line arguments
  if (parse_args(argc, argv, &cmd) != EXIT_SUCCESS) {
    LOG_ERROR("Failed to parse arguments");
    return EXIT_FAILURE;
  }

  // Execute the command
  int result = execute_cmd(&cmd);

  // Cleanup allocated memory
  if (cmd.fields) {
    str_vec_free(&cmd.fields);
  }
  if (cmd.array) {
    str_vec_free(&cmd.array);
  }

  return result;
}
