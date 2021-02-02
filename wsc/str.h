#ifndef STR_H
#define STR_H
#include "macro.h"

typedef struct {
  char* ptr;
  ui32 size;
} str;

extern str* str_init(ui32 size);
extern str* str_new(const char* text);
extern void str_del(const str*);
extern str* str_add(ui32 count, str* s, ...);
extern str* str_multiply(const str*, ui32);
extern char* str_get(const str*);

#endif  // STR_H
