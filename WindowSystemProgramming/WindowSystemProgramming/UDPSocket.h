#pragma once
#include "BaseSocket.h"
#include "OverlappedEx.h"
#include "SocketAddress.h"

class UDPSocket : public BaseSocket
{
private:
	OverlappedEx overlappedEx;
	TCHAR recvBuf[512];

public:
	UDPSocket() { memset(recvBuf, '\0', sizeof(recvBuf)); }
	~UDPSocket() {}

	virtual int CreateSocket() override;
	int Bind(const SocketAddress& sockAddr);
	int AsyncSendTo(int bytes, const SocketAddress& socketAddress);
	int AsyncReceiveFrom(int bytes, const SocketAddress& socketAddress);
};