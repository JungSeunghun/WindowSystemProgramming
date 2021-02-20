#include "SocketAddress.h"

SocketAddress::SocketAddress()
{
	memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons(0);
}

SocketAddress::SocketAddress(ULONG address, USHORT portNum)
{
	memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = htonl(address);
	socketAddress.sin_port = htons(portNum);
}

SocketAddress::SocketAddress(PCWSTR ipAddress, USHORT portNum)
{
	memset(socketAddress.sin_zero, 0, sizeof(socketAddress.sin_zero));
	socketAddress.sin_family = AF_INET;
	InetPton(AF_INET, ipAddress, &socketAddress.sin_addr.s_addr);
	socketAddress.sin_port = htons(portNum);
}

//SOCKADDR_IN SocketAddress::GetSocketAddressFromDomainName(const string& domainName) {
//	size_t pos = domainName.find_last_of(':');
//	string host;
//	string service;
//	if (pos != string::npos) {
//		host = domainName.substr(0, pos);
//		service = domainName.substr(pos + 1);
//	}
//	else {
//		host = domainName;
//		//use default port...
//		service = "0";
//	}
//
//	addrinfo hint;
//	memset(&hint, 0, sizeof(hint));
//	hint.ai_family = AF_INET;
//
//	addrinfo* result;
//	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
//	if (error != 0 && result != nullptr) {
//		Error::ReportError("GetIp");
//		return NULL;
//	}
//
//	while (!result->ai_addr && result->ai_next)
//		result = result->ai_next;
//
//	if (!result->ai_addr)
//		return nullptr;
//
//	SOCKADDR_IN ret = static_cast<SOCKADDR_IN>(result->ai_addr);
//	freeaddrinfo(result);
//
//	return ret;
//}
//
//string SocketAddress::AddressToString() const
//{
//	char destinationBuffer[128];
//	InetNtop(socketAddress.sin_family, &socketAddress.sin_addr, (PWSTR)destinationBuffer, sizeof(destinationBuffer));
//	string s = destinationBuffer + ':' + ntohs(socketAddress.sin_port);
//	return s;
//}