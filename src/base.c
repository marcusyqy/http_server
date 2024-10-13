#include "base.h"
#include "assert.h"
#include "stdio.h"
#include <stdarg.h>
#include <string.h>

#if _MSC_VER
# define align_of(T) __alignof(T)
#elif __clang__
# define align_of(T) __alignof(T)
#elif __GNUC__
# define align_of(T) __alignof__(T)
#else
# error [Build Error] align_of(T) is not defined for this compiler.
#endif

struct Da_Header *base_dynamic_array_header(void *ptr) {
  assert(ptr);
  struct Da_Header *header = (struct Da_Header *)ptr;
  return header - 1;
}

void *base_dynamic_array_reserve(void *ptr, size_t object_size, size_t capacity) {
  struct Da_Header *header = base_dynamic_array_header(ptr);
  if(capacity != 0) {
    if(header->capacity < capacity) {
      header = realloc(header, sizeof(struct Da_Header) + object_size * capacity);
      header->capacity = capacity;
    }
  }
  return header + 1;
}

void *base_dynamic_array_create(size_t object_size, size_t capacity) {
  struct Da_Header *ptr = NULL;
  if(!capacity) {
    ptr = malloc(sizeof(struct Da_Header));
    ptr->capacity = 0;
  } else {
    size_t size_to_allocate = sizeof(struct Da_Header) + object_size * capacity;
    ptr = malloc(size_to_allocate);
    ptr->capacity = capacity;
  }
  ptr->count = 0;
  return ptr + 1;
}

void base_dynamic_array_free(void *ptr) {
  free(base_dynamic_array_header(ptr));
}

size_t base_dynamic_array_count(void *ptr) {
  struct Da_Header *header = base_dynamic_array_header(ptr);
  return header->count;
}

size_t base_dynamic_array_capacity(void *ptr) {
  return base_dynamic_array_header(ptr)->capacity;
}

void *base_dynamic_array_grow(void *ptr, size_t object_size) {
  struct Da_Header *header = base_dynamic_array_header(ptr);
  header->capacity = header->capacity ? header->capacity * 2 : 1;
  header = realloc(header, sizeof(struct Da_Header) + header->capacity * object_size);
  return header + 1;
}

StringBuilder string_builder_create(size_t capacity) {
  return (StringBuilder) {
    .count = 0,
    .capacity = capacity,
    .buffer = capacity != 0 ? malloc(capacity * sizeof(char)) : NULL,
  };
}

void string_builder_free(StringBuilder string[static 1]) {
  free(string->buffer);
  *string = (StringBuilder){0}; // zero out for reuse
}

void string_builder_reserve_size(StringBuilder string[static 1], size_t size) {
  if(size > string->capacity) {
    while(size > string->capacity) {
      string->capacity = string->capacity ? string->capacity * 2 : 1;
    }
    string->buffer = realloc(string->buffer, string->capacity);
  }
}

void string_builder_append_char(StringBuilder string[static 1], char c) {
  if(string->count + 1 > string->capacity) {
    string->capacity = string->capacity ? string->capacity * 2 : 1;
    string->buffer = realloc(string->buffer, string->capacity);
  }
  string->buffer[string->count++] = c;
}

void string_builder_append_null(StringBuilder string[static 1]) {
  string_builder_append_char(string, 0);
  string->count--;
}

void string_builder_append_cstr(StringBuilder string[static 1], const char cstring[static 1]) {
  const char *c = cstring;
  for(;*c != 0; ++c) string_builder_append_char(string, *c);
  string_builder_append_null(string);
}


void string_builder_append_fmt(StringBuilder string[static 1], const char cstring[static 1], ...) {
  va_list list;
  va_start(list, cstring);
  int length = vsnprintf(NULL, 0, cstring, list);
  string_builder_reserve_size(string, string->count + length + 1);
  vsnprintf(string->buffer + string->count, length + 1, cstring, list);
  string->count += length;
  va_end(list);
}

void string_builder_append_strview(StringBuilder string[static 1], StringView view) {
  for(size_t i = 0; i < view.count; ++i) string_builder_append_char(string, view.buffer[i]);
  string_builder_append_null(string);
}

StringView cstr_to_strview(const char *string) {
  return (StringView) {
    .count = strlen(string),
    .buffer = string,
  };
}

StringView str_to_view(StringBuilder string) {
  return (StringView) {
    .count = string.count,
    .buffer = string.buffer,
  };
}

StringView strview_null() {
  return (StringView) {0};
}


