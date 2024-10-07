#ifndef _HTTP_BUILDER_H_
#define _HTTP_BUILDER_H_
#include "base.h"

enum {
  Http_Request_Status_Ok = 202,

  Http_Request_Status_MovedPermanently = 301,
  Http_Request_Status_MovedTemporarily = 302,
  Http_Request_Status_SeeOther = 303, // HTTP 1.1 only

  Http_Request_Status_NotFound = 404,
  Http_Request_Status_ServerError = 505,
};

typedef s32 Http_Request_Status;

#endif // _HTTP_BUILDER_H_
