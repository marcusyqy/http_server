#ifndef _BASE_H_
#define _BASE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

struct Da_Header {
  size_t count;
  size_t capacity;
};

void *base_dynamic_array_create(size_t object_size, size_t capacity);
void base_dynamic_array_free(void *ptr);

size_t base_dynamic_array_count(void *ptr);
size_t base_dynamic_array_capacity(void *ptr);
void *base_dynamic_array_grow(void *ptr, size_t object_size);
void *base_dynamic_array_reserve(void *ptr, size_t object_size, size_t capacity);
struct Da_Header *base_dynamic_array_header(void *ptr);


#define da_create(Type, capacity) base_dynamic_array_create(sizeof(Type), capacity)
#define da_free(ptr) base_dynamic_array_free(ptr)
#define da_count(ptr) base_dynamic_array_count(ptr)
#define da_capacity(ptr) base_dynamic_array_capacity(ptr)
#define da_reserve(ptr, size) do { (ptr) = base_dynamic_array_reserve((ptr), sizeof((ptr)[0]), size); } while(0)

// assert(sizeof(item) == sizeof((array)[0]));
#define da_append(array, item) do { \
  struct Da_Header *header = base_dynamic_array_header(array); \
  if(header->count + 1 >= header->capacity) { \
    (array) = base_dynamic_array_grow((array), sizeof((array)[0])); \
  }\
  /* we need to re-get it here */ \
  header = base_dynamic_array_header(array);\
  array[header->count++] = item; \
} while(0) \


#define da_pop(array) do { \
  struct Da_Header *header = base_dynamic_array_header(array); \
  header->count = header->count ? header->count - 1 : header->count; \
} while(0) \

#define da_resize_no_shrink(array, size) do { \
  da_reserve(array, size); \
  base_dynamic_array_header(array)->count = size; \
} while(0) \

typedef struct {
  size_t count;
  size_t capacity;
  char *buffer;
} StringBuilder;

typedef struct {
  size_t count;
  const char *buffer;
} StringView;

static inline bool is_alpha(char c) {
  // Looking at the ascii table, we can simply just or with 32 to upgrade 'A' to 'a'.
  char new_c = c | 32;
  return new_c >= 'a' && new_c <= 'z';
}

static inline bool is_number(char c) {
  return c >= '0' && c <= '9';
}

static inline bool is_alpha_numeric(char c) {
  // Looking at the ascii table, we can simply just or with 32 to upgrade 'A' to 'a'.
  return is_alpha(c) || is_number(c);
}

static inline bool is_whitespace(char c) {
  return isspace(c);
}

StringBuilder string_builder_create(size_t capacity);
void string_builder_free(StringBuilder string[static 1]);

void string_builder_reserve_size(StringBuilder string[static 1], size_t size);

void string_builder_append_char(StringBuilder string[static 1], char c);
void string_builder_append_null(StringBuilder string[static 1]);
void string_builder_append_cstr(StringBuilder string[static 1], const char cstring[static 1]);
void string_builder_append_fmt(StringBuilder string[static 1], const char cstring[static 1], ...);


StringView cstr_to_strview(const char *string);
StringView str_to_view(StringBuilder string);
StringView strview_null();

#endif // _BASE_H_

