#include "http/parser.h"
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include <stdio.h>

static size_t http_get_next_whitespace(const char buffer[static 1], size_t cursor, size_t length) {
  for(;cursor < length; ++cursor) {
    if(is_whitespace(buffer[cursor])) break;
  }
  return cursor;
}

static size_t http_get_next_non_whitespace(const char buffer[static 1], size_t cursor, size_t length) {
  for(;cursor < length; ++cursor) {
    if(!is_whitespace(buffer[cursor])) break;
  }
  return cursor;
}

static void http_try_parse_method(Http_Parser parser[static 1]) {
  size_t cursor = http_get_next_whitespace(parser->buffer, parser->cursor, parser->length);
  if(cursor == parser->cursor) {
    parser->method = Http_Method_Unknown;
  } else if(cursor - parser->cursor == 3 && strncmp("GET", parser->buffer + parser->cursor, 3) == 0) {
    parser->method = Http_Method_GET;
  } else {
    parser->method = Http_Method_Unsupported;
  }

  if(parser->method == Http_Method_Unknown ||
      parser->method == Http_Method_Unsupported) {
    // there shouldn't be anything else to parse now
    return;
  }

  /// GET <request-target>["?"<query>] HTTP/1.1
  // read body
  // query + /...? (need to disect).
  parser->cursor = http_get_next_non_whitespace(parser->buffer, cursor, parser->length);
  cursor = http_get_next_whitespace(parser->buffer, parser->cursor, parser->length);
  if(parser->cursor != cursor) {
    printf("body: ");
    for(size_t i = parser->cursor; i < cursor; ++i) {
      printf("%c", parser->buffer[i]);
    }
    printf("\n");
  }

  // read last thing
  parser->cursor = http_get_next_non_whitespace(parser->buffer, cursor, parser->length);
  cursor = http_get_next_whitespace(parser->buffer, parser->cursor, parser->length);

  if(parser->cursor != cursor) {
    printf("HTTP: ");
    for(size_t i = parser->cursor; i < cursor; ++i) {
      printf("%c", parser->buffer[i]);
    }
    printf("\n");
  }

  // must end with this?
  if(cursor - parser->cursor == 8 && strncmp("HTTP/1.1", parser->buffer + parser->cursor, 8) == 0) {
    parser->method = Http_Method_Unsupported; // something went wrong
  }
  parser->cursor = cursor;

  printf("Rest : %s\n", parser->buffer + cursor);
}

Http_Parser *http_create_parser(const char buffer[static 1], size_t length) {
  Http_Parser *parser = malloc(sizeof(Http_Parser));
  memset(parser, 0, sizeof(Http_Parser));
  parser->buffer = buffer;
  parser->length = length;
  http_try_parse_method(parser);
  return parser;
}

void http_free_parser(Http_Parser parser[static 1]) {
  // free pages here?
  free(parser);
}

Http_Token *http_get_next_token(Http_Parser parser[static 1]) {
  return NULL;
}
