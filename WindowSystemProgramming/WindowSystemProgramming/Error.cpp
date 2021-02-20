#include "Error.h"

void Error::ReportError(const TCHAR* description)
{
	TCHAR* message;
	DWORD errorNum = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&message,
		0, NULL);

	_tprintf(_T("Error(%s) : %d- %s\n"), description, errorNum, message);
	LocalFree(message);
}

int Error::GetLastError() {
	return WSAGetLastError();
}
