#include "os/file.common.h"

#include "file.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

static void os_linux_struct_tm_convert_to_time_point(
    const struct tm *tm, TimeInfo *info) {
  *info = (TimeInfo){0};
  info->week_day  = tm->tm_wday;
  info->month_day = tm->tm_mday;
  info->month     = tm->tm_mon + 1;
  info->year      = tm->tm_year + 1900;
  info->hour      = tm->tm_hour;
  info->minute    = tm->tm_min;
  info->second    = tm->tm_sec;
}

bool os_get_current_time(TimeInfo *info) {
  time_t t = time(NULL);
  struct tm *tm = gmtime(&t);
  os_linux_struct_tm_convert_to_time_point(tm, info);
  return true;
}

bool os_file_get_write_time_gmt(const char *file_name, TimeInfo *info) {
  assert(file_name);
  assert(info);

  struct stat attrib;
  stat(file_name, &attrib);
  struct tm *tm = gmtime(&(attrib.st_ctime));

  if(tm == NULL) return false;
  os_linux_struct_tm_convert_to_time_point(tm, info);
  return true;
}
