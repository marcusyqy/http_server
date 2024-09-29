#include "base.h"
#include "assert.h"
#include "stdio.h"

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
