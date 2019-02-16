#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <process.h>
#include <Iphlpapi.h>

#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")


class UDPSocket{

public:

	UDPSocket(char* ip, short port);
	~UDPSocket();
	void sendto(_In_ void* buf, _In_ int sendSize);
	int recvfrom(_Out_ char* buf, _In_ int recvSize);
private:

	SOCKET sk;
	SOCKADDR_IN addrOFsend;

	char* mc_ip;
	short mc_port;
};

#endif