#ifndef SOCKET_NETWORK_H
#define SOCKET_NETWORK_H
#include <../LibVideoConference/SQLService.h>
#include "NetworkUtils.h"
#include "CPacketer.h"
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <exception>
#include <mutex>
#include <votebot.h>
typedef enum
{
	C_ERROR,
	C_SUCCESS,
	C_SET_VOTED,
	C_CHECK_USER,
	C_CHCCK_USER_VOTED,
	C_REQUEST_GROUPS,
	C_REQUEST_USER_MEETING,
	C_REQUEST_USER_DATA,
	C_REQUEST_USERS_DATA,
	C_REQUEST_MEETING_MEM_DATA,
	C_UPDATE_USER_NAME,
	C_UPDATE_USER_PASSWORD,
	C_END_MEETING,
	C_CREATE_MEETING,
	C_TOTAL_INS,
}CodeBehavior;
class SocketBase
{
public:
											SocketBase();
											~SocketBase();
	bool									errorState() const;
	std::string								lastError() const;
private:
	static int								useCount;
	static WSADATA							wsd;
protected:
	CFragmentTransfer						transfer;
	SOCKET									sock;
	SOCKADDR_IN								sockAddr;
	std::string								ip;
	int										port;
	bool									hasError;
	std::string								lastErrorStr;
};
class SockServer : public SocketBase
{
public:
											SockServer(std::string address = "0.0.0.0:6666");
											~SockServer();

	bool									start();
	void									stop();
	std::string								getIP() const;
	int										getPort() const;
	int										getTotalConnection() const;
	SockServer&								setIP			(_In_ std::string ip);
	SockServer&								setPort(int port);
private:
	void									accept();
	void									commandProcessor(_In_ SOCKET client);
	void									closeProcessor(_In_ SOCKET client);
	CodeBehavior							send			(_In_ SOCKET client, _In_ std::string data);
	CodeBehavior							recv			(_In_ SOCKET client, _Out_ std::string &recvBuf);
	CSQLService								db;
	VoteBot									vb;
	std::map<SOCKET, std::thread*>			clientList;
	std::thread								*acceptThread;
	std::mutex								sync_mtx;
	SOCKADDR_IN								sockAddr;
	bool									quit;
};
class SockClient : public SocketBase
{
public:
											SockClient(std::string address = "127.0.0.1:5531");
											~SockClient();

	SockClient&								setHost(std::string address);
	bool									connect(const char *newIPPort = nullptr); //specify to nullptr (Use saved IP/Port)
	std::string								sendCommand(std::string);//blocking
private:
	CodeBehavior							send(std::string data);
	CodeBehavior							recv(std::string &recvBuf); 
	std::mutex								sync_mtx;
};
#endif
