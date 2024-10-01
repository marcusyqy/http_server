#ifndef _OS_FILE_H_
#define _OS_FILE_H_

#include "file.common.h"

#if defined(_WIN32)
#include "win32/file.h"
#else
#include "linux/file.h"
#endif

bool os_file_get_write_time_gmt(const char *file_name, TimeInfo *info);
bool os_get_current_time(TimeInfo *info);

#endif // _OS_FILE_H_





