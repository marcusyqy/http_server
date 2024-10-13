#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "base.h"
#include "base.c"

#include "os/net.h"
#include "os/net.c"

#include "os/file.h"
#include "os/file.c"

#include <time.h>

#include "http/parser.h"
#include "http/parser.c"

#include "http/builder.h"
#include "http/builder.c"

#define PORT 3000
typedef struct {
  StringView extension_type;
  bool is_text_type;
} ContentType;

// because they cannot agree man.
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
               const char filename[static 1],
               bool is_text) {
  FILE *file = open_file(filename, is_text ? "r" : "rb");
  if(file == NULL) return false;
  fseek(file, 0L, SEEK_END);
  size_t filelen = ftell(file);
  rewind(file);

  string_builder_reserve_size(file_buffer, sizeof(char) * filelen + 1);
  if(!fread(file_buffer->buffer, sizeof(char), filelen, file)) return false;
  file_buffer->count = filelen; // length of the entire file.
  string_builder_append_null(file_buffer);
  fclose(file);
  return true;
}

ContentType extension_to_content_type(StringView extension) {
  printf("%.*s : %zu\n", (int)extension.count, extension.buffer, extension.count);
  switch(extension.count) {
    case 2:
      if(strncmp(extension.buffer, "js", 2) == 0) return (ContentType) {.extension_type = cstr_to_strview("text/javascript"), .is_text_type = true };
      break;
    case 3:
      if(strncmp(extension.buffer, "css", 3) == 0) return (ContentType) {.extension_type = cstr_to_strview("text/css"), .is_text_type = true };
      if(strncmp(extension.buffer, "ico", 3) == 0) return (ContentType) {.extension_type = cstr_to_strview("image/png"), .is_text_type = false };
      if(strncmp(extension.buffer, "png", 3) == 0) return (ContentType) {.extension_type = cstr_to_strview("image/png"), .is_text_type = false };
      if(strncmp(extension.buffer, "jpg", 3) == 0) return (ContentType) {.extension_type = cstr_to_strview("image/jpeg"), .is_text_type = false };
      break;
    case 4:
      if(strncmp(extension.buffer, "html", 4) == 0) return (ContentType) {.extension_type = cstr_to_strview("text/html"), .is_text_type = true };
      if(strncmp(extension.buffer, "jpeg", 4) == 0) return (ContentType) {.extension_type = cstr_to_strview("image/jpeg"), .is_text_type = false };
      break;
  }

  return (ContentType) {
      .extension_type = cstr_to_strview("text/plain"),
      .is_text_type = true
  };
}


void build_response_packet(StringBuilder response_packet[static 1],
                           StringBuilder file_buffer[static 1],
                           StringView filename,
                           int major, int minor) {
  // clear current buffer.
  response_packet->count = 0;
  file_buffer->count     = 0;

  fprintf(stdout, "Building response packet for: %.*s\n", (int)filename.count, filename.buffer);

  // get extension here
  s32 ext_start = -1; // we can build something with no extension? Just text @TODO
  s32 ext_end   = 0;
  for(; ext_end < filename.count; ++ext_end) {
    if(filename.buffer[ext_end] == '.') ext_start = ext_end;
  }
  ext_start++; // don't want to count the .
  ContentType content_type = extension_to_content_type((StringView){ .count = ext_end - ext_start, .buffer = filename.buffer + ext_start });

  char null_terminated_filename[128] = {0};
  assert(128 >= filename.count);
  strncpy(null_terminated_filename, filename.buffer, filename.count);

  if(!read_file(file_buffer, null_terminated_filename, content_type.is_text_type)) {
    http_builder_response_header(response_packet, Http_Response_Status_NotFound, 1, 0);
    http_builder_set_content_type(response_packet,
        cstr_to_strview("text/plain"), strview_null());
    http_builder_set_content(response_packet, cstr_to_strview("Page not Found"));
    return;
  }

  TimeInfo current_time = {0};
  assert(os_get_current_time(&current_time));

  TimeInfo file_time = {0};
  assert(os_file_get_write_time_gmt(null_terminated_filename, &file_time));

  http_builder_response_header(response_packet, Http_Response_Status_Ok, major, minor);
  http_builder_set_connection_status(response_packet, &(Http_ResponseKeepAlive){ .timeout = 5, .max = 1000 });
  http_builder_set_date_gmt(response_packet, &current_time);
  http_builder_set_last_modified_gmt(response_packet, &file_time);
  http_builder_set_content_type(response_packet, content_type.extension_type, content_type.is_text_type ? cstr_to_strview("UTF-8") : strview_null());
  http_builder_set_content(response_packet, str_to_view(*file_buffer));
}

int main(int arg_count, char **args) {
  // parse this soon?
  (void)arg_count;
  (void)args;

  // build response header
  StringBuilder response_packet = {0};
  StringBuilder file_buffer = {0};


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

  for(;;) {
    NetConnection new_connection = os_net_accept(&connection);
    if (!os_net_connection_valid(&new_connection)) {
      os_print_last_error("ERROR on accept");
      os_net_end_connection(&new_connection);
      goto quit;
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
      assert(parser.initial_line.path.count);
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

      // @TODO: parse this later.
      fprintf(stdout, "Rest : %s\n", parser.buffer + parser.cursor);

      // putting this here so that i can quit from the browser??
      if(strlen("/quit") == parser.initial_line.path.count
         && strncmp(parser.initial_line.path.buffer, "/quit", parser.initial_line.path.count) == 0) {
        http_free_parser(&parser);
        goto quit;
      }

      build_response_packet(&response_packet, &file_buffer,
          parser.initial_line.path.count != 1 ? (StringView) {
            .count = parser.initial_line.path.count - 1,
            .buffer = parser.initial_line.path.buffer + 1,
          } : cstr_to_strview("index.html"),
          parser.initial_line.version_major, parser.initial_line.version_minor);
      fprintf(stdout, "buffer(%zu): %.*s\n", response_packet.count, (int)response_packet.count, response_packet.buffer);
      // should build request thingy
      int write_size = os_net_send_sync(&new_connection, response_packet.buffer, response_packet.count);
      if (write_size < 0) os_print_last_error("ERROR writing to socket");

      http_free_parser(&parser);
    }
    os_net_end_connection(&new_connection);
  }

quit:
  free(buffer);
  string_builder_free(&response_packet);
  string_builder_free(&file_buffer);

  printf("Connection closing\n");
  os_net_end_connection(&connection);
  os_net_exit();
  return 0;
}

