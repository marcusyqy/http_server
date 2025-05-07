#include "file.h"
#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)

#include <windows.h>

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

#elif defined(__linux__) || defined(__APPLE__)

#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

static void os_linux_struct_tm_convert_to_time_point(
    const struct tm tm[static 1], TimeInfo info[static 1]) {
  *info = (TimeInfo){0};
  info->week_day  = tm->tm_wday;
  info->month_day = tm->tm_mday;
  info->month     = tm->tm_mon + 1;
  info->year      = tm->tm_year + 1900;
  info->hour      = tm->tm_hour;
  info->minute    = tm->tm_min;
  info->second    = tm->tm_sec;
}

#else // LINUX
#error platform not supported
#endif


bool os_file_get_write_time_gmt(const char file_name[static 1], TimeInfo info[static 1]) {
#if defined(_WIN32)
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
#elif defined(__linux__) || defined(__APPLE__)
  struct stat attrib;
  stat(file_name, &attrib);
  struct tm *tm = gmtime(&(attrib.st_ctime));

  if(tm == NULL) return false;
  os_linux_struct_tm_convert_to_time_point(tm, info);
  return true;
#else
#error not supported
#endif
}


bool os_get_current_time(TimeInfo info[static 1]) {
#if defined(_WIN32)
  SYSTEMTIME st_utc = {0};
  GetSystemTime(&st_utc);
  os_win32_systemtime_convert_to_time_point(&st_utc, info);
  return true;
#elif defined(__linux__) || defined(__APPLE__)
  time_t t = time(NULL);
  struct tm *tm = gmtime(&t);
  os_linux_struct_tm_convert_to_time_point(tm, info);
  return true;
#else
#error not supported
#endif
}
