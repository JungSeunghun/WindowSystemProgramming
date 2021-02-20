#include "UDPSocket.h"

int UDPSocket::CreateSocket() {
	baseSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (baseSocket == INVALID_SOCKET) {
		Error::ReportError(_T("UDPSocket::CreateSocket"));
		return Error::GetLastError();
	}

	return NO_ERROR;
}

int UDPSocket::Bind(const SocketAddress& sockAddr)
{
	int error = bind(baseSocket, (PSOCKADDR)&sockAddr.GetSocketAddress(), sockAddr.GetSize());
	if (error != 0)
	{
		Error::ReportError(_T("UDPSocket::Bind"));
		return Error::GetLastError();
	}

	return NO_ERROR;
}

int UDPSocket::AsyncSendTo(int bytes, const SocketAddress& socketAddress)
{
	DWORD sendBytes = 0;
	DWORD flag = 0;
	
	overlappedEx.state = State::SENDED;
	overlappedEx.wsaBuf.buf = (CHAR*)recvBuf;
	overlappedEx.wsaBuf.len = bytes;
	int error = WSASendTo(baseSocket, &overlappedEx.wsaBuf, 1, &sendBytes, flag,
		(PSOCKADDR)&socketAddress.GetSocketAddress(), socketAddress.GetSize(), &overlappedEx, NULL);
	if (error == SOCKET_ERROR) {
		Error::ReportError(_T("UDPSocket::Receive"));
		return -Error::GetLastError();
	}

	memset(recvBuf, '\0', sizeof(recvBuf));

	return NO_ERROR;
}

int UDPSocket::AsyncReceiveFrom(int bytes, const SocketAddress& socketAddress)
{
	DWORD recvBytes = 0;
	DWORD flag = 0;
	int addrLen = socketAddress.GetSize();

	overlappedEx.state = State::RECVED;
	overlappedEx.wsaBuf.buf = (CHAR*)recvBuf;
	overlappedEx.wsaBuf.len = sizeof(recvBuf);

	int error = WSARecvFrom(baseSocket, &overlappedEx.wsaBuf, 1, &recvBytes, &flag,
		(PSOCKADDR)&socketAddress.GetSocketAddress(), &addrLen, &overlappedEx, NULL);
	if (error == SOCKET_ERROR) {
		Error::ReportError(_T("UDPSocket::Receive"));
		return -Error::GetLastError();
	}

	return NO_ERROR;
}
