#include "http/parser.h"
#include <stdlib.h>
#include <string.h>
#include "base.h"

static size_t get_next_whitespace(const char *buffer, size_t cursor, size_t length) {
  for(;cursor < length; ++cursor) {
    if(is_whitespace(buffer[cursor])) break;
  }
  return cursor;
}

static size_t get_next_non_whitespace(const char *buffer, size_t cursor, size_t length) {
  for(;cursor < length; ++cursor) {
    if(!is_whitespace(buffer[cursor])) break;
  }
  return cursor;
}

static void http_try_parse_method(Http_Parser *parser) {
  size_t cursor = get_next_whitespace(parser->buffer, parser->cursor, parser->length);
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
  parser->cursor = get_next_non_whitespace(parser->buffer, cursor, parser->length);
  cursor = get_next_whitespace(parser->buffer, parser->cursor, parser->length);
  // query + /...?

  // read last thing
  parser->cursor = get_next_non_whitespace(parser->buffer, cursor, parser->length);
  cursor = get_next_whitespace(parser->buffer, parser->cursor, parser->length);
  // must end with this?
  if(cursor - parser->cursor == 8 && strncmp("HTTP/1.1", parser->buffer + parser->cursor, 8) == 0) {
    parser->method = Http_Method_Unsupported; // something went wrong
  }
}

Http_Parser *http_create_parser(const char *buffer, size_t length) {
  Http_Parser *parser = malloc(sizeof(Http_Parser));
  memset(parser, 0, sizeof(Http_Parser));
  parser->buffer = buffer;
  parser->length = length;
  http_try_parse_method(parser);
  return parser;
}

void http_free_parser(Http_Parser *parser) {
  // free pages here?
  free(parser);
}

Http_Token *http_get_next_token(Http_Parser* parser) {
  return NULL;
}
