#pragma once
#include "TCPSocket.h"
#include "SocketAddress.h"

class TCPListenSocket : public TCPSocket
{
public:
	int Bind(const SocketAddress& sockAddr);
	int Listen(int backLog);
};

