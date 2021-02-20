#include "SocketFunction.h"

PVOID SocketFunction::GetFunctionEx(SOCKET socket, GUID guidFn) {
	if (socket == INVALID_SOCKET)
		return NULL;

	PVOID function = NULL;
	GUID guid = guidFn;
	DWORD dwBytes = 0;
	LONG ret = ::WSAIoctl (
		socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid),
		&function, sizeof(function),
		&dwBytes, NULL, NULL
	);
	if (ret == SOCKET_ERROR)
	{
		cout << "WSAIoctl failed, code : " << WSAGetLastError() << endl;
		return NULL;
	}

	return function;
}