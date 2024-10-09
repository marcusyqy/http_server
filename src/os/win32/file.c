
#include "os/file.common.h"
#include "file.h"
#include <windows.h>
#include <stdio.h>
#include <assert.h>


static void os_win32_systemtime_convert_to_time_point(
    const SYSTEMTIME st[static 1], TimeInfo info[static 1]) {
  *info = (TimeInfo){0};
  info->year      = st->wYear;
  info->month     = st->wMonth;
  info->week_day  = st->wDayOfWeek + 1;
  info->month_day = st->wDay;
  info->hour      = st->wHour;
  info->minute    = st->wMinute;
  info->second    = st->wSecond;
}


bool os_get_current_time(TimeInfo info[static 1]) {
  SYSTEMTIME st_utc = {0};
  GetSystemTime(&st_utc);
  os_win32_systemtime_convert_to_time_point(&st_utc, info);
  return true;
}

bool os_file_get_write_time_gmt(const char file_name[static 1], TimeInfo info[static 1]) {
  assert(file_name);
  assert(info);

  HANDLE h_file = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  FILETIME ft_create, ft_access, ft_write;
  if (!h_file || !GetFileTime(h_file, &ft_create, &ft_access, &ft_write)) {
      fprintf(stderr, "Something went wrong\n");
      return false;
  }

  SYSTEMTIME st_utc = {0};
  FileTimeToSystemTime(&ft_write, &st_utc);
  os_win32_systemtime_convert_to_time_point(&st_utc, info);
  CloseHandle(h_file);
  return true;
}
