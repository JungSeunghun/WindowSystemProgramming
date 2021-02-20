#pragma once
#include "TCPSocket.h"
#include "OverlappedEx.h"
#include "SocketAddress.h"
#include "SocketFunction.h"

class TCPConnectSocket : public TCPSocket
{
private:
	OverlappedEx overlappedEx;

	LPFN_CONNECTEX connectEx;
	LPFN_ACCEPTEX acceptEx;
	LPFN_DISCONNECTEX disconnectEx;

	TCHAR connectBuf[512];
	TCHAR acceptBuf[512];
	TCHAR recvBuf[512];

	bool isCreateIOCP;
public:
	TCPConnectSocket();
	~TCPConnectSocket();

	bool IsCreateIOCP() { return isCreateIOCP; }
	void SetIsCreateIOCP(bool create) { isCreateIOCP = create; }

	int AsyncConnect(const SocketAddress& socketAddress);
	int AsyncAccept(SOCKET listenSocket);
	int AsyncDisconnect();
	int AsyncSend(DWORD bytes);
	int AsyncRecv();
};

