#ifndef CORE_H
#define CORE_H
#include "cvector.h"

SVEC_DEF
typedef struct {
  svec_t* entry;
} entry_ref_t;
VEC_DEF(entry_ref_t, entry)

#endif  // !CORE_H
