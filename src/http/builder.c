#include "builder.h"
#include <string.h>
#include <stdio.h>

static const char *http_response_status_to_string_name(Http_Response_Status status) {
  switch(status) {
    case Http_Response_Status_Ok:
      return "OK";
    case Http_Response_Status_BadRequest:
      return "Bad Request";
    case Http_Response_Status_NotFound:
      return "Not Found";
  }
  return "Bad Request";
}

static const char *http_get_day(int day) {
  switch(day) {
    case 1:  return "Mon";
    case 2:  return "Tue";
    case 3:  return "Wed";
    case 4:  return "Thu";
    case 5:  return "Fri";
    case 6:  return "Sat";
    case 7:
    case 0:  return "Sun";
    default: return "Unknown";
  }
}

static const char *http_get_month(int month) {
  switch(month) {
    case 1:  return "Jan";
    case 2:  return "Feb";
    case 3:  return "Mar";
    case 4:  return "Apr";
    case 5:  return "May";
    case 6:  return "Jun";
    case 7:  return "Jul";
    case 8:  return "Aug";
    case 9:  return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "Unknown";
  }
}


void http_builder_response_header(StringBuilder string[static 1],
                                  Http_Response_Status status, int major_version, int minor_version) {
  string_builder_append_fmt(string,
    "HTTP/%d.%d %d %s\r\n",
    major_version, minor_version,
    status, http_response_status_to_string_name(status));
}

void http_builder_set_connection_status(StringBuilder string[static 1],
                                        Http_ResponseKeepAlive *params) {
  if(params != NULL) {
    string_builder_append_fmt(string,
      "Connection: Keep-Alive\r\n"
      "Keep-Alive: timeout=%d, max=%d\r\n",
      params->timeout, params->max);
  } else {
    string_builder_append_cstr(string, "Connection: close\r\n");
  }
}

void http_builder_set_content_type(StringBuilder string[static 1],
                                   StringView file_extension,
                                   StringView char_format) {
  string_builder_append_fmt(string,
    "Content-Type: text/%.*s; charset=%.*s\r\n",
    file_extension.count, file_extension.buffer,
    char_format.count, char_format.buffer);
}

void http_builder_set_date_type_header_gmt(StringBuilder string[static 1],
                                           StringView header_name,
                                           TimeInfo time[static 1]) {
  string_builder_append_fmt(string,
    "%.*s: %s, %02d %s %04d %02d:%02d:%02d GMT\r\n",
    header_name.count, header_name.buffer,
    http_get_day(time->week_day), time->month_day, http_get_month(time->month), time->year, time->hour, time->minute, time->second);
}

void http_builder_set_date_gmt(StringBuilder string[static 1],
                               TimeInfo time[static 1]) {
  http_builder_set_date_type_header_gmt(string, cstr_to_strview("Date"), time);
}

void http_builder_set_last_modified_gmt(StringBuilder string[static 1],
                                        TimeInfo time[static 1]) {
  http_builder_set_date_type_header_gmt(string, cstr_to_strview("Last-Modified"), time);
}

void http_builder_set_content(StringBuilder string[static 1],
                              StringView content) {
  string_builder_append_fmt(string,
    "Content-Length: %lu\r\n"
    "\r\n"
    "%.*s\r\n",
    content.count, content.count, content.buffer);
}

