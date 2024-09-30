#ifndef _OS_NET_H_
#define _OS_NET_H_

#include "common.h"

#if defined(_WIN32)
#include "win32/net.h"
#else
#include "linux/net.h"
#endif

#endif // _OS_NET_H_
