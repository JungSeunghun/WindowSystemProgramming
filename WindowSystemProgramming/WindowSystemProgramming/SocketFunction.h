#pragma once
#include "Define.h"

class SocketFunction {
public:
	static PVOID GetFunctionEx(SOCKET socket, GUID guidFn);
};

