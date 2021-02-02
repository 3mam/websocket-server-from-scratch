#include "str.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

str* str_init(ui32 size) {
  str* self = NEW(str);
  self->size = size;
  self->ptr = MAKE(char, self->size);
  return self;
}

str* str_new(const char* txt) {
  str* self = str_init((ui32)strlen(txt));
  strncpy(self->ptr, txt, self->size);
  return self;
}

void str_del(const str* self) {
  free((void*)self->ptr);
  free((void*)self);
}

str* str_add(ui32 count, str* s, ...) {
  va_list ap;
  ui32 size = 0;
  str* tmp = s;

  va_start(ap, s);
  for (ui32 i = 0; i < count; i++) {
    size += tmp->size;
    tmp = va_arg(ap, str*);
  }
  va_end(ap);

  str* self = str_init(size);

  va_start(ap, s);
  tmp = s;
  ui32 possition = 0;
  for (ui32 i = 0; i < count; i++) {
    for (ui32 j = 0; j < tmp->size; j++) {
      self->ptr[possition] = tmp->ptr[j];
      possition++;
    }
    tmp = va_arg(ap, str*);
  }
  va_end(ap);
  return self;
}

str* str_multiply(const str* str1, ui32 multiplay) {
  str* self = str_init(str1->size * multiplay);
  for (ui32 i = 0; i < multiplay; i++)
    strcat(self->ptr, str1->ptr);
  return self;
}

char* str_get(const str* self) {
  return self->ptr;
}
