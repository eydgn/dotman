#include "common.h"

#include <string.h>
#include <ctype.h>

scope_type_t scope_str_to_enum(const char* scp) {
  if (!(strcmp("dot", scp))) {
    return SCOPE_DOT;
  }
  if (!(strcmp("pkg", scp))) {
    return SCOPE_PKG;
  }
  if (!(strcmp("bld", scp))) {
    return SCOPE_BLD;
  }
  return SCOPE_UNKNOWN;
}

const char* scope_enum_to_str(scope_type_t scope) {
  switch (scope) {
    case SCOPE_DOT:
      return "dot";
    case SCOPE_PKG:
      return "pkg";
    case SCOPE_BLD:
      return "bld";
    case SCOPE_UNKNOWN:
      return "unknown";
    case SCOPE_NONE:
      return "none";
  }
  return NULL;
}

char* trim(char* str) {
  while (*str && isspace((unsigned char) *str)) {
    str++;
  }
  char* end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char) *end)) {
    *end-- = '\0';
  }
  return str;
}
