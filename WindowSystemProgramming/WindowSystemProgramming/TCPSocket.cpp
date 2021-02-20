#include "TCPSocket.h"

int TCPSocket::CreateSocket() {
	baseSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (baseSocket == INVALID_SOCKET) {
		Error::ReportError(_T("TCPSocket::CreateSocket"));
		return Error::GetLastError();
	}

	return NO_ERROR;
}
