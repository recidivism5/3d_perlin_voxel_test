#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#define HAVE_STRUCT_TIMESPEC
#ifdef _WIN32 // Win32 platforms use winsock.
#include <winsock2.h>
typedef int socklen_t;
#define close closesocket
#else // Non windows platforms use berkeley sockets.  To allow the program to compile under these platforms, some items have to be redefined.
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#define SOCKET uint
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif
#include <pthread.h>

float own_position[3];
float friend_position[3];

bool bConnectionSuccess;

void* start_client(void* args);
void client_send_position();