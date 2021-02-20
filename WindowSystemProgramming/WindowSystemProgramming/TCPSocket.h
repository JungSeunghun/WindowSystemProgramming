#pragma once
#include "BaseSocket.h"

class TCPSocket : public BaseSocket
{
public:
	virtual int CreateSocket() override;
};
