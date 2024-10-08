#ifndef _OS_FILE_COMMON_H_
#define _OS_FILE_COMMON_H_

#include "base.h"
#include <stdbool.h>

typedef struct {
  s32 second; // 0 - 59
  s32 minute; // 0 - 59
  s32 hour;   // 0 - 23
  s32 year;   //
  s32 month;  // 1 - 12

  s32 month_day; // 1 - 31
  s32 week_day;  // 1 - 7
} TimeInfo;

#endif // _OS_FILE_COMMON_H_
