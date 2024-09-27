#include <stdio.h>

#include "os/net.h"
#include "os/net.c"

#define PORT "8000"

int main(void) {
  int return_code = 0;
  printf("hello world\n");

  // @TODO: move this back into somewhere the os windows will use.
  NetInitResult response = os_win32_net_init();
  if(response != NetInitResult_Ok && response != NetInitResult_AlreadyInitialized) {
    // error here.
    printf("`os_win32_net_init` failed with: %d\n", response);
    return -1;
  }

  // server code.
	struct addrinfo *result = NULL;
  struct addrinfo *ptr    = NULL;
  struct addrinfo  hints  = {0};

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family   = AF_INET;      // using ipv4
	hints.ai_socktype = SOCK_STREAM;  // specify stream socket
	hints.ai_protocol = IPPROTO_TCP;  // TCP protocol
	hints.ai_flags    = AI_PASSIVE;   //

	// Resolve the local address and port to be used by the server
	int call_result = getaddrinfo(NULL, PORT, &hints, &result);
	if (call_result != 0) {
			printf("getaddrinfo failed: %d\n", call_result);
      return_code = 1;
      goto end;
	}

  SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if(listen_socket == INVALID_SOCKET) {
    printf("Error at socket(): %d\n", WSAGetLastError());
    return_code = 1;
    goto end;
  }


end:
  os_win32_net_exit();
  return return_code;
}
