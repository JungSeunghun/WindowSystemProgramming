#include "TCPConnectSocket.h"

TCPConnectSocket::TCPConnectSocket()
	:connectEx(nullptr), acceptEx(nullptr), disconnectEx(nullptr), isCreateIOCP(false)
{
	memset(connectBuf, '\0', sizeof(connectBuf));
	memset(acceptBuf, '\0', sizeof(acceptBuf));
	memset(recvBuf, '\0', sizeof(recvBuf));
}

TCPConnectSocket::~TCPConnectSocket() {
}

int TCPConnectSocket::AsyncConnect(const SocketAddress& socketAddress) {
	DWORD connectBytes;

	if (overlappedEx.baseSocket == nullptr)
		overlappedEx.baseSocket = this;

	connectEx = static_cast<LPFN_CONNECTEX>(SocketFunction::GetFunctionEx(baseSocket, WSAID_CONNECTEX));
	BOOL error = connectEx(baseSocket, (PSOCKADDR)&socketAddress.GetSocketAddress(), socketAddress.GetSize(),
		connectBuf, 0, &connectBytes, &overlappedEx);
	if (error == FALSE)	{
		if (WSAGetLastError() != WSA_IO_PENDING) {
			Error::ReportError(_T("TCPSocket::AsyncConnect"));
			return Error::GetLastError();
		}
	}
	return NO_ERROR;
}

int TCPConnectSocket::AsyncAccept(SOCKET listenSocket) {

	DWORD acceptBytes;

	if(overlappedEx.baseSocket == nullptr)
		overlappedEx.baseSocket = this;

	acceptEx = static_cast<LPFN_ACCEPTEX>(SocketFunction::GetFunctionEx(baseSocket, WSAID_ACCEPTEX));
	BOOL error = acceptEx(listenSocket, baseSocket, acceptBuf,
		0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&acceptBytes, &overlappedEx);
	if (error == FALSE)	{
		if (WSAGetLastError() != WSA_IO_PENDING) {
			Error::ReportError(_T("TCPSocket::AsyncAccept"));
			return Error::GetLastError();
		}
	}
	return NO_ERROR;
}

int TCPConnectSocket::AsyncDisconnect() {
	disconnectEx = static_cast<LPFN_DISCONNECTEX>(SocketFunction::GetFunctionEx(baseSocket, WSAID_DISCONNECTEX));
	
	BOOL error = disconnectEx(baseSocket, NULL, TF_REUSE_SOCKET, 0);
	return NO_ERROR;
}

int TCPConnectSocket::AsyncSend(DWORD bytes) {
	DWORD flag = 0;
	DWORD sendBytes = 0;

	recvBuf[bytes] = 0;

	overlappedEx.state = State::SENDED;
	overlappedEx.wsaBuf.buf = (CHAR*)recvBuf;
	overlappedEx.wsaBuf.len = bytes;
	cout << " *** Client(" << baseSocket << ") sent : " << (const char*)recvBuf << endl;

	int error = WSASend(baseSocket, &overlappedEx.wsaBuf, 1, &sendBytes, flag, &overlappedEx, NULL);
	if (error == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			Error::ReportError(_T("TCPSocket::AsyncSend"));
			return Error::GetLastError();
		}		
	}

	memset(recvBuf, '\0', sizeof(recvBuf));

	return NO_ERROR;
}


int TCPConnectSocket::AsyncRecv() {
	DWORD flag = 0;
	DWORD recvBytes = 0;

	overlappedEx.state = State::RECVED;
	overlappedEx.wsaBuf.buf = (CHAR*)recvBuf;
	overlappedEx.wsaBuf.len = sizeof(recvBuf);

	int error = WSARecv(baseSocket, &overlappedEx.wsaBuf, 1, &recvBytes, &flag, &overlappedEx, NULL);
	if (error == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			Error::ReportError(_T("TCPSocket::AsyncRecv"));
			return Error::GetLastError();
		}		
	}

	return NO_ERROR;
}