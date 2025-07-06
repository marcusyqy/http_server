#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "base.h"
#include "os/file.h"

enum {
  Http_Response_Status_Ok       = 200,

  // Redirection messages
  // Http_Response_Status_MovedPermanently = 301,
  // Http_Response_Status_MovedTemporarily = 302,
  // Http_Response_Status_SeeOther         = 303, // HTTP 1.1 only

  // Error messages
  Http_Response_Status_BadRequest   = 400,
  // Http_Response_Status_Unauthorized = 401,
  Http_Response_Status_NotFound     = 404,
  // Http_Response_Status_ServerError  = 505,
};

typedef s32 Http_Response_Status;
typedef s32 Http_Response_ContentType;

typedef struct {
  int timeout;
  int max;
} Http_ResponseKeepAlive;

void http_response_builder_append_header(StringBuilder string[static 1],
    Http_Response_Status status, int major_version, int minor_version);

void http_response_builder_set_connection_status(StringBuilder string[static 1],
                                        Http_ResponseKeepAlive *params);


void http_response_builder_set_date_type_header_gmt(StringBuilder string[static 1],
                                           StringView header_name,
                                           TimeInfo time[static 1]);
void http_response_builder_set_date_gmt(StringBuilder string[static 1],
                               TimeInfo time[static 1]);
void http_response_builder_set_last_modified_gmt(StringBuilder string[static 1],
                                        TimeInfo time[static 1]);

void http_response_builder_set_content_type(StringBuilder string[static 1],
                                   StringView file_extension,
                                   StringView char_format);
void http_response_builder_set_content(StringBuilder string[static 1],
                              StringView content);

#endif // _HTTP_RESPONSE_H_
