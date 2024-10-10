#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include "base.h"

/// GET / HTTP/1.1
/// Host: localhost:3000
/// Connection: keep-alive
/// Cache-Control: max-age=0
/// sec-ch-ua: "Chromium";v="129", "Not=A?Brand";v="8"
/// sec-ch-ua-mobile: ?0
/// sec-ch-ua-platform: "Linux"
/// Upgrade-Insecure-Requests: 1
/// User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/129.0.0.0 Safari/537.36
/// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
/// Sec-Fetch-Site: none
/// Sec-Fetch-Mode: navigate
/// Sec-Fetch-User: ?1
/// Sec-Fetch-Dest: document
/// Accept-Encoding: gzip, deflate, br, zstd
/// Accept-Language: en-US,en;q=0.9
/// If-Modified-Since: Mon, 30 Sep 2024 03:29:29 GMT

enum {
  Http_Method_Unsupported = -1,
  Http_Method_Unknown = 0,
  Http_Method_GET
};

enum {
  Http_RequestField_Unsupported = -1,
  Http_RequestField_Host
};

typedef s32 Http_RequestField;
typedef s32 Http_Method;

typedef struct Http_Token {
  Http_RequestField field;
  StringView content;
} Http_Token;

typedef struct Http_GET_Params {
} Http_GET_Params;

typedef struct Http_Parser {
  const char *buffer;
  size_t length;

  size_t cursor;

  Http_Method method;
  Http_GET_Params method_param;

  Http_Token *queue;
} Http_Parser;



Http_Parser *http_create_parser(const char buffer[static 1], size_t length);
void http_free_parser(Http_Parser parser[static 1]);

Http_Token *http_get_next_token(Http_Parser parser[static 1]);

#endif // _HTTP_PARSER_H_
