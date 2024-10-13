#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include "base.h"
/// - ( Initial Request Line )
/// GET / HTTP/1.1
///
/// - ( Optional Header lines )
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
  Http_RequestHeaderLine_Unsupported = -1,
};

typedef s32 Http_Method;

typedef struct {
  Http_Method method;
  StringView path;
  int version_major;
  int version_minor;
} Http_InitialLine;

typedef struct {
  const char *buffer;
  size_t length;

  size_t cursor;

  /// Parse the initial line
  Http_InitialLine initial_line;
} Http_Parser;


void http_init_parser(Http_Parser parser[static 1], const char buffer[static 1], size_t length);
void http_free_parser(Http_Parser parser[static 1]);

#endif // _HTTP_PARSER_H_
