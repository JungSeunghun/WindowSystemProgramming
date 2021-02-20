#pragma once
#include "Define.h"
class SocketAddress
{
private:
	SOCKADDR_IN socketAddress;

public:	
	SocketAddress();
	SocketAddress(ULONG adress, USHORT portNum);
	SocketAddress(PCWSTR ipAddress, USHORT portNum);

	const SOCKADDR_IN& GetSocketAddress() const { return socketAddress; }
	SOCKADDR_IN& GetSocketAddress() { return socketAddress; }
	int GetSize() const { return sizeof(socketAddress); }
	
	/*static SOCKADDR_IN GetSocketAddressFromDomainName(const string& domainName);
	
	string AddressToString() const;*/
};
