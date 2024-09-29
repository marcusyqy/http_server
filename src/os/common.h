#ifndef _OS_COMMON_H_
#define _OS_COMMON_H_

#include "base.h"

enum {
  NetInitResult_Ok = 0,
  NetInitResult_NotReady,
  NetInitResult_NotSupported, // not suppported on this architecture.
  NetInitResult_AlreadyInitialized,
  NetInitResult_LimitReached,
  NetInitResult_Unknown,
};

enum {
  NetConnectionResult_Ok = 0,
};

typedef s32 NetConnectionResult;
typedef s32 NetInitResult;


#endif //  _OS_COMMON_H_

