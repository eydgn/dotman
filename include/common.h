#ifndef COMMON_H
#define COMMON_H

#include "cvector.h"

#define DOTMAN_VERSION "v0.1.0"

typedef enum {
  SCOPE_NONE = 0,
  SCOPE_DOT,
  SCOPE_PKG,
  SCOPE_BLD,
  SCOPE_SET,
  SCOPE_UNKNOWN,
} scope_type_t;

scope_type_t scope_str_to_enum(const char* scp);
const char*  scope_enum_to_str(scope_type_t scope);
char*        trim(char* str);

// Vectors
DEFINE_STR_VECTOR

// Logging macros
#define COLOR_RESET "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"

#define LOG_ERROR(msg, ...)                                                      \
  fprintf(stderr, COLOR_RED "[ERROR] %s:%d:%s()\n" COLOR_RESET "   ↳ " msg "\n", \
          __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LOG_WARN(msg, ...)                                                         \
  fprintf(stderr, COLOR_YELLOW "[WARN] %s:%d:%s()\n" COLOR_RESET "   ↳ " msg "\n", \
          __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LOG_INFO(msg, ...)                                                       \
  fprintf(stdout, COLOR_BLUE "[INFO] %s:%d:%s()\n" COLOR_RESET "   ↳ " msg "\n", \
          __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define EXTRACT_ARRAY(struct_ptr, data)               \
  char* saveptr;                                      \
  char* element = strtok_r(data, ",", &saveptr);      \
  while (element != NULL) {                           \
    element = trim(element);                          \
    if (*element == '\0') {                           \
      LOG_ERROR("Empty element in the array field."); \
      return EXIT_FAILURE;                            \
    }                                                 \
    if (str_vec_push((struct_ptr)->array, element)) { \
      LOG_ERROR("Failed to push element to array.");  \
      return EXIT_FAILURE;                            \
    }                                                 \
    element = strtok_r(NULL, ",", &saveptr);          \
  }
#endif  // !COMMON_H
