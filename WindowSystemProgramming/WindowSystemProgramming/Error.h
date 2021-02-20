#pragma once
#include "Define.h"

class Error
{
public:
	static void ReportError(const TCHAR* description);
	static int GetLastError();
};

