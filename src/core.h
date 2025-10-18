#ifndef CORE_H
#define CORE_H
#include "cvector.h"

#define COLOR_GREEN "\033[32m"
#define COLOR_RED   "\033[31m"
#define COLOR_RESET "\033[0m"

SVEC_DEF
typedef struct {
  svec_t* entry;
} entry_ref_t;
VEC_DEF(entry_ref_t, entry)

#endif  // !CORE_H
