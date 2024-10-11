#include "base.h"
#include "base.c"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
  int x;
  int y;
  int z;
} Event;

typedef struct {
  Event *items;
  size_t count;
  size_t capacity;
} Events;

int main(int argc, char **argv) {
  Event *events = da_create(Event, 10);
  printf("cap: %zu\n", da_count(events));
  printf("cap: %zu\n", da_capacity(events));
  da_reserve(events, 100);

  Event event = {0};
  for(int i = 0; i < 100; ++i) {
    event.x = i;
    event.y = i;
    event.z = i;
    da_append(events, event);
    printf("%zu %zu\n", da_count(events), da_capacity(events));
  }

  printf("Verify\n");
  for(int i = 0; i < 100; ++i) {
    assert(events[i].x == i ||
           events[i].y == i ||
           events[i].z == i);
    printf("%d : Event { %d, %d, %d }\n", i, events[i].x, events[i].y, events[i].z);
  }

  da_free(events);
}
