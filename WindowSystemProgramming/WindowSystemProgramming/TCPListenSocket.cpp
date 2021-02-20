#include "TCPListenSocket.h"

int TCPListenSocket::Bind(const SocketAddress& sockAddr) {
	int error = bind(baseSocket, (PSOCKADDR)&sockAddr.GetSocketAddress(), sockAddr.GetSize());
	if (error != 0)
	{
		Error::ReportError(_T("UDPSocket::Bind"));
		return Error::GetLastError();
	}

	return NO_ERROR;
}

int TCPListenSocket::Listen(int backLog) {
	int err = listen(baseSocket, backLog);
	if (err != 0) {
		Error::ReportError(_T("TCPSocket::Listen"));
		return Error::GetLastError();
	}
	return NO_ERROR;
}