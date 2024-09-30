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

enum {
  NetConnectionResult_Error = -1,
  NetConnectionResult_Disconnect = 0,
};


enum {
  NetSocketType_None = 0,
  NetSocketType_SendRecv = 1 << 0,
};


typedef s32 NetConnectionRecvResult;
typedef s32 NetConnectionSendResult;
typedef s32 NetConnectionResult;
typedef s32 NetInitResult;
typedef s32 NetSocketType;


#endif //  _OS_COMMON_H_

