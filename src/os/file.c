
#if defined(_WIN32)
#include "win32/file.c"
#else
#include "linux/file.c"
#endif
