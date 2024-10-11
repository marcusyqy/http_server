#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "base.h"
#include "base.c"

#include "os/net.h"
#include "os/net.c"

#include "os/file.h"
#include "os/file.c"

#include <time.h>

#include "http/http.h"
#include "http/http.c"

#define PORT 3000

const char *http_get_day(int day) {
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

const char *http_get_month(int month) {
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

FILE* open_file(const char *file, const char *param) {
  FILE *fptr = NULL;
#if defined(_WIN32) // just to suppress warnings.
  fopen_s(&fptr, file, param);
#elif defined(__linux__)
  fptr = fopen(file, param);
#else
#error Platform not supported
#endif
  return fptr;
}

int main(int arg_count, char **args) {
  (void)arg_count;
  (void)args;

  FILE *file = open_file("index.html", "r");
  assert(file);
  fseek(file, 0L, SEEK_END);
  size_t filelen = ftell(file);
  rewind(file);

  printf("%zu\n", filelen);

  char *filebuf = malloc(sizeof(char) * filelen + 1);
  assert(fread(filebuf, sizeof(char), filelen, file));
  filebuf[filelen] = 0;
  fclose(file);

  TimeInfo current_time = {0};
  assert(os_get_current_time(&current_time));

  TimeInfo file_time = {0};
  assert(os_file_get_write_time_gmt("index.html", &file_time));


  const char *content = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "Date: %s, %02d %s %04d %02d:%02d:%02d GMT\r\n"
                        "Last-Modified: %s, %02d %s %04d %02d:%02d:%02d GMT\r\n"
                        "Content-Length: %lu\r\n"
                        "\r\n"
                        "%s\r\n";

  int printlen = snprintf(NULL, 0, content,
                    http_get_day(current_time.week_day), current_time.month_day, http_get_month(current_time.month), current_time.year, current_time.hour, current_time.minute, current_time.second,
                    http_get_day(file_time.week_day), file_time.month_day, http_get_month(file_time.month), file_time.year, file_time.hour, file_time.minute, file_time.second,
                    filelen,
                    filebuf);

  char *printbuf = malloc(sizeof(char)*printlen);
  printlen = snprintf(printbuf, printlen, content,
                    http_get_day(current_time.week_day), current_time.month_day, http_get_month(current_time.month), current_time.year, current_time.hour, current_time.minute, current_time.second,
                    http_get_day(file_time.week_day), file_time.month_day, http_get_month(file_time.month), file_time.year, file_time.hour, file_time.minute, file_time.second,
                    filelen,
                    filebuf);
  printf("%s\n", printbuf);

  NetInitResult response = os_net_init();
  if(response != NetInitResult_Ok && response != NetInitResult_AlreadyInitialized) {
    // error here.
    printf("`os_net_init` failed with: %d\n", response);
    return -1;
  }
  // Resolve the local address and port to be used by the server
  NetConnection connection = os_net_start_connection(NULL, PORT);

  os_net_listen(&connection, 5);

#define DEFAULT_BUFLEN 4096
  int buflen = DEFAULT_BUFLEN;
  char *buffer = malloc(buflen);

  NetConnection new_connection = os_net_accept(&connection);
  if (os_net_connection_valid(&new_connection)) {
    os_print_last_error("ERROR on accept");
  }

  for(;;) {
    int read_size = os_net_recv_sync(&new_connection, buffer, buflen);

    if(read_size == NetConnectionResult_Disconnect) { printf("Exiting gracefully\n"); break; }
    if(read_size == NetConnectionResult_Error) {
      os_print_last_error("ERROR reading from socket");
      break;
    }
    assert(read_size <= buflen);
    buffer[read_size] = 0;
    fprintf(stdout, "Here is the message(%d): %s\n", read_size, buffer);

    Http_Parser parser = {0};
    http_init_parser(&parser, buffer, read_size); // this attempts to reads the initial line.
    fprintf(stdout, "Path: ");
    for(size_t i = 0; i < parser.initial_line.path.count; ++i) {
      fprintf(stdout, "%c", parser.initial_line.path.buffer[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "Http Version: %d.%d\n",
        parser.initial_line.version_major, parser.initial_line.version_minor);
    fprintf(stdout, "Rest : %s\n", parser.buffer + parser.cursor);

    // should build request thingy
    int write_size = os_net_send_sync(&new_connection, printbuf, printlen);
    if (write_size < 0) os_print_last_error("ERROR writing to socket");

    http_free_parser(&parser);
  }

  free(buffer);

  printf("Connection closing\n");
  os_net_end_connection(&new_connection);
  os_net_end_connection(&connection);
  os_net_exit();
  return 0;
}

