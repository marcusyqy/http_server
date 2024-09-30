#include <stdio.h>

#include "os/net.h"
#include "os/net.c"
#include "base.h"
#include "base.c"
#include <time.h>

#include "http/http.h"
#include "http/http.c"

#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>

#define PORT 3000
#define DEFAULT_BUFLEN 512



#if 0
int main(int arg_count, char **args) {
  (void)arg_count;
  (void)args;

  NetInitResult response = os_net_init();
  if(response != NetInitResult_Ok && response != NetInitResult_AlreadyInitialized) {
    // error here.
    printf("`os_net_init` failed with: %d\n", response);
    return -1;
  }
  // Resolve the local address and port to be used by the server
  NetConnection *connection = os_net_start_connection(NULL, PORT);

  while(listen(connection->socket, SOMAXCONN) != SOCKET_ERROR) {
    // accept a connection here.
    SOCKET client_socket = accept(connection->socket, NULL, NULL);
    if(client_socket == INVALID_SOCKET) {
      continue;
    }

    char recv_buffer[DEFAULT_BUFLEN];
    int recv_buflen = DEFAULT_BUFLEN - 1;

    int recv_result = 0;
    for(;;) {
      recv_result = recv(client_socket, recv_buffer, recv_buflen, 0);
      if(recv_result < 0) {
        printf("`recv` failed with %d\n", WSAGetLastError());
        break;
      }

      if(recv_result == 0) {
        break;
      }

      recv_buffer[recv_result] = '\0';
      printf("Bytes received: %d\n", recv_result);
      printf("Received bytes: %s\n", recv_buffer);

      /*
      // Echo the buffer back to the sender
      int send_result = send(client_socket, recv_buffer, recv_result, 0);
      if (send_result == SOCKET_ERROR) {
      printf("send failed: %d\n", WSAGetLastError());
      closesocket(client_socket);
      WSACleanup();
      return 1;
      }
      printf("Bytes sent: %d\n", send_result);
      */
    }

    printf("Connection closing\n");
    int end_result = shutdown(client_socket, SD_SEND);
    if(end_result == SOCKET_ERROR) {
      printf("shutdown failed: %d\n", WSAGetLastError());
    }
    closesocket(client_socket);
    break;
  }

  os_net_end_connection(connection);
  os_net_exit();
  return 0;
}
#endif


void error(const char *msg) {
    perror(msg);
    exit(1);
}

const char *get_day(int day) {
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

const char *get_month(int month) {
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


struct tm get_file_modified_time(char *file_path) {
  struct stat attrib;
  stat(file_path, &attrib);
  char date[10];
  return *gmtime(&(attrib.st_ctime));
}

int main(int argc, char *argv[]) {
  FILE *file = fopen("../index.html", "r");
  assert(file);
  fseek(file, 0L, SEEK_END);
  size_t filelen = ftell(file);
  rewind(file);

  char *filebuf = malloc(sizeof(char) * filelen);
  assert(fread(filebuf, sizeof(char) * filelen, 1, file));

  time_t t = time(NULL);
  struct tm tm  = *gmtime(&t);
  struct tm ftm = get_file_modified_time("../index.html");

  const char *content = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "Date: %s, %02d %s %04d %02d:%02d:%02d GMT\r\n"
                        "Last-Modified: %s, %02d %s %04d %02d:%02d:%02d GMT\r\n"
                        "Content-Length: %lu\r\n"
                        "\r\n"
                        "%s\r\n";

  int printlen = snprintf(NULL, 0, content,
                    get_day(tm.tm_wday), tm.tm_mday, get_month(tm.tm_mon + 1), tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
                    get_day(ftm.tm_wday), ftm.tm_mday, get_month(ftm.tm_mon + 1), ftm.tm_year + 1900, ftm.tm_hour, ftm.tm_min, ftm.tm_sec,
                    filelen,
                    filebuf);

  char *printbuf = malloc(sizeof(char)*printlen);
  printlen = snprintf(printbuf, printlen, content,
                    get_day(tm.tm_wday), tm.tm_mday, get_month(tm.tm_mon + 1), tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
                    get_day(ftm.tm_wday), ftm.tm_mday, get_month(ftm.tm_mon + 1), ftm.tm_year + 1900, ftm.tm_hour, ftm.tm_min, ftm.tm_sec,
                    filelen,
                    filebuf);

  printf("%s", printbuf);

  os_net_init();
  NetConnection *connection = os_net_start_connection(NULL, PORT);
  assert(connection);

  os_net_listen(connection, 5);

  NetConnection *new_connection = os_net_accept(connection);
  if (new_connection == NULL) error("ERROR on accept");
  for(;;) {
    char buffer[DEFAULT_BUFLEN+1] = {0};
    int buflen = DEFAULT_BUFLEN;
    int read_size = os_net_recv_sync(new_connection, buffer, buflen);

    if(read_size == NetConnectionResult_Disconnect) { printf("Exiting gracefully\n"); break; }
    if(read_size == NetConnectionResult_Error)      { printf("ERROR reading from socket(%d):%s\n", errno, strerror(errno)); break; }

    buffer[DEFAULT_BUFLEN] = 0;
    printf("Here is the message(%d): %s\n", read_size, buffer);

    int write_size = os_net_send_sync(new_connection, printbuf, printlen);
    if (write_size < 0) error("ERROR writing to socket");
  }

  os_net_end_connection(new_connection);

  os_net_end_connection(connection);
  free(printbuf);
  free(filebuf);
  os_net_exit();
  return 0;
}
