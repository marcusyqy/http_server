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

bool read_file(StringBuilder file_buffer[static 1],
               const char *filename) {
  FILE *file = open_file(filename, "r");
  if(file == NULL) return false;
  fseek(file, 0L, SEEK_END);
  size_t filelen = ftell(file);
  rewind(file);

  string_builder_reserve_size(file_buffer, sizeof(char) * filelen + 1);
  assert(fread(file_buffer->buffer, sizeof(char), filelen, file));
  file_buffer->count = filelen; // length of the entire file.
  string_builder_append_null(file_buffer);
  fclose(file);
  return true;
}

int main(int arg_count, char **args) {
  (void)arg_count;
  (void)args;

  StringBuilder file_buffer = {0};
  assert(read_file(&file_buffer, "index.html"));

  TimeInfo current_time = {0};
  assert(os_get_current_time(&current_time));

  TimeInfo file_time = {0};
  assert(os_file_get_write_time_gmt("index.html", &file_time));

  // build response header
  StringBuilder response_packet = {0};
  http_builder_response_header(&response_packet, Http_Response_Status_Ok, 1, 0);
  http_builder_set_connection_status(&response_packet, &(Http_ResponseKeepAlive){ .timeout = 5, .max = 1000 });
  http_builder_set_date_gmt(&response_packet, &current_time);
  http_builder_set_last_modified_gmt(&response_packet, &file_time);
  http_builder_set_content_type(&response_packet, cstr_to_strview("html"), cstr_to_strview("UTF-8"));
  http_builder_set_content(&response_packet, str_to_view(file_buffer));

  fprintf(stdout, "buffer(%zu): %.*s\n", response_packet.count, (int)response_packet.count, response_packet.buffer);

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
  if (!os_net_connection_valid(&new_connection)) {
    os_print_last_error("ERROR on accept");
  }

  for(;;) {
    int read_size = os_net_recv_sync(&new_connection, buffer, buflen);

    if(read_size == NetConnectionResult_Disconnect) {
      fprintf(stdout, "Exiting gracefully\n");
      break;
    }

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

    // HTTP/1.0 or HTTP/1.1
    assert(parser.initial_line.version_major == 1 &&
       (parser.initial_line.version_minor == 0 ||
        parser.initial_line.version_minor == 1));

    fprintf(stdout, "Rest : %s\n", parser.buffer + parser.cursor);

    // should build request thingy
    int write_size = os_net_send_sync(&new_connection, response_packet.buffer, response_packet.count);
    if (write_size < 0) os_print_last_error("ERROR writing to socket");

    http_free_parser(&parser);
  }

  free(buffer);
  string_builder_free(&response_packet);
  string_builder_free(&file_buffer);

  printf("Connection closing\n");
  os_net_end_connection(&new_connection);
  os_net_end_connection(&connection);
  os_net_exit();
  return 0;
}

