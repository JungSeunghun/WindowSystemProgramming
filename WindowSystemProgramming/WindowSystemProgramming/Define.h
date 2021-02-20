#pragma once
#define NOMINMAX

#include <Winsock2.h>
#include <Mswsock.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>
#include <tchar.h>
#include <ws2tcpip.h>

#include "Singleton.h"
#include "Error.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#ifndef STATUS_LOCAL_DISCONNECT
#	define STATUS_LOCAL_DISCONNECT	((NTSTATUS)0xC000013BL)	//ERROR_NETNAME_DELETED
#endif
#ifndef STATUS_REMOTE_DISCONNECT
#	define STATUS_REMOTE_DISCONNECT	((NTSTATUS)0xC000013CL)	//ERROR_NETNAME_DELETED
#endif
#ifndef STATUS_CONNECTION_RESET
#	define STATUS_CONNECTION_RESET	((NTSTATUS)0xC000020DL)	//ERROR_NETNAME_DELETED
#endif
#ifndef STATUS_CANCELLED
#	define STATUS_CANCELLED			((NTSTATUS)0xC0000120L)	//ERROR_OPERATION_ABORTED
#endif

#define IOKEY_LISTEN	1
#define IOKEY_CHILD		2

#define TM_PROG_EXIT			WM_USER + 1
#define TM_SOCK_CONNECTED		WM_USER + 2
#define TM_SOCK_DISCONNECTED	WM_USER + 3
#define TM_SOCK_ACCEPTED		WM_USER + 4

#define	POOL_MAX_SIZE	4
#define	POOL_MIN_SIZE	2
#define PERMIT_INC_CNT	2
