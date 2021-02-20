#pragma once
#include "Define.h"

class BaseSocket
{
protected:
	SOCKET baseSocket;
public:
	BaseSocket() : baseSocket(INVALID_SOCKET) {}
	~BaseSocket() {}

	void CloseSocket() { closesocket(baseSocket); }
	SOCKET GetSocket() { return baseSocket; }

	virtual int CreateSocket() = 0;
};

