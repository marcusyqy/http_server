#ifndef _OS_FILE_H_
#define _OS_FILE_H_

#include "file.common.h"

#if defined(_WIN32)
#include "win32/file.h"
#else
#include "linux/file.h"
#endif

#endif // _OS_FILE_H_





