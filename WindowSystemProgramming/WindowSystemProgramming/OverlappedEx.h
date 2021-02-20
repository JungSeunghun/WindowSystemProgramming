#pragma once
#include "Define.h"

class BaseSocket;

enum class State {
	NONE,
	RECVED,
	SENDED,
};

struct OverlappedEx : OVERLAPPED
{
	BaseSocket* baseSocket;
	WSABUF wsaBuf;
	State state;

	OverlappedEx() {
		memset(this, 0, sizeof(*this));
		wsaBuf.buf = nullptr;
		wsaBuf.len = 0;
		baseSocket = nullptr;
		state = State::NONE;
	}
	~OverlappedEx() {
		if (baseSocket != nullptr)
			baseSocket = nullptr;
		if (wsaBuf.buf != nullptr)
			wsaBuf.buf = nullptr;
	}
};
