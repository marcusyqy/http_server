#include "http/parser.h"
#include <string.h>
#include "base.h"

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
  Http_InitialLine *initial_line = &parser->initial_line;
  *initial_line = (Http_InitialLine){0}; // zero first.
  if(cursor == parser->cursor) {
    initial_line->method = Http_Method_Unknown;
  } else if(cursor - parser->cursor == 3 && strncmp("GET", parser->buffer + parser->cursor, 3) == 0) {
    initial_line->method = Http_Method_GET;
  } else {
    initial_line->method = Http_Method_Unsupported;
  }

  if(initial_line->method == Http_Method_Unknown ||
      initial_line->method == Http_Method_Unsupported) {
    // there shouldn't be anything else to parse now
    return;
  }

  /// GET <request-target>["?"<query>] HTTP/1.1
  // read body
  // query + /...? (need to disect).
  parser->cursor = http_get_next_non_whitespace(parser->buffer, cursor, parser->length);
  cursor = http_get_next_whitespace(parser->buffer, parser->cursor, parser->length);
  if(parser->cursor != cursor) {
    initial_line->path = (StringView) { .count = cursor - parser->cursor, .buffer = parser->buffer + parser->cursor };
  }

  // read last thing
  parser->cursor = http_get_next_non_whitespace(parser->buffer, cursor, parser->length);
  cursor = http_get_next_whitespace(parser->buffer, parser->cursor, parser->length);
  if(cursor - parser->cursor == 8 &&
      strncmp("HTTP/", parser->buffer + parser->cursor, 5) == 0) {
    initial_line->version_major = *(parser->buffer + parser->cursor + 5) - '0';
    initial_line->version_minor = *(parser->buffer + parser->cursor + 7) - '0';
  } else {
    // something went wrong
    initial_line->method = Http_Method_Unsupported;
  }

  parser->cursor = cursor;
}

void http_init_parser(Http_Parser parser[static 1], const char buffer[static 1], size_t length) {
  *parser = (Http_Parser){ .buffer= buffer, .length = length };
  http_try_parse_method(parser);
  // this needs to be checking for CRLF instead.
  parser->cursor = http_get_next_non_whitespace(parser->buffer, parser->cursor, parser->length);
}

void http_free_parser(Http_Parser parser[static 1]) {
  // free pages here eventually.
}

