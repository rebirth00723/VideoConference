#include "UDPSocket.h"


UDPSocket::~UDPSocket(){

	if (SOCKET_ERROR != sk)
		closesocket(sk);
	printf("%d", WSAGetLastError());
	WSACleanup();
}
UDPSocket::UDPSocket(char* ip, short port){

	SOCKADDR_IN addrOFbind;
	IP_MREQ mreq;
	int addrlen;
	int ttl;
	int loop;
	int reval;

	mc_ip = ip;
	mc_port = port;

	

	sk = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sk){
		cout << "socket ERROR\n";
		this->~UDPSocket();
	}

	addrOFbind.sin_family = AF_INET;
	addrOFbind.sin_addr.s_addr =htonl(INADDR_ANY);//
	addrOFbind.sin_port = htons(mc_port);

	addrOFsend.sin_family = AF_INET;
	addrOFsend.sin_port = htons((short)mc_port);
	addrOFsend.sin_addr.s_addr = inet_addr(mc_ip);

	bind(sk, (SOCKADDR*)&addrOFbind, sizeof(addrOFbind));

	mreq.imr_multiaddr.s_addr = inet_addr(mc_ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	ttl = 1;
	loop = 0;


	setsockopt(sk, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	setsockopt(sk, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl));
	//setsockopt(sk, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loop, sizeof(loop));

}

void UDPSocket::sendto(_In_ void* buf, _In_ int sendSize = 0){

	char* mbuf = (char*)buf;
	if (!sendSize)
		sendSize = strlen(mbuf);

	::sendto(sk, mbuf, sendSize, 0, (SOCKADDR *)&addrOFsend, sizeof(addrOFsend));

}
int UDPSocket::recvfrom(_Out_ char* buf, _In_ int recvSize){ //void*

	int bytes;
	int addrlen;
	SOCKADDR addrOFin;

	addrlen = sizeof(SOCKADDR);

	bytes = ::recvfrom(sk, buf, recvSize, 0, &addrOFin, &addrlen);

	return bytes;
}