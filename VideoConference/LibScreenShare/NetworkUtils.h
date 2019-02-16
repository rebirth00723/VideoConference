#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H
#include "CPacketQueue.h"
#include <utility>
#include <string>
#include <array>
#include <list>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#define TIME_OUT_INFINITE -1
#pragma warning(disable:4996)
/*
Note that the code of this file won't initialize socket library by itself.
This file must be used along with TCPService.h.
The thing is you have to create TCPService's object for starting socket environment up
Or initialize it by yourself!
*/
typedef enum
{
	FILE_TYPE_NONE,
	FILE_TYPE_FILE,
	FILE_TYPE_DIR,
	FILE_TYPE_PIC
}FILE_TYPE;
typedef struct
{
	std::wstring name;
	FILE_TYPE type;
}File;
typedef enum
{
	CNT_NO_ERR,
	CNT_LOCAL_SOCKET_ERR,
	CNT_OUTPUT_FILE_FAILED,
	CNT_FILE_NOT_EXISTED,
	CNT_CONNECTED_TIME_OUT,
	CNT_CONNECTION_LOST
}FTPCNT_ERR;
bool parseIPPort(const std::string IPPort, std::string &IPAddress, int &Port);
std::wstring makeIPPort(const std::wstring IP, int Port);
std::string makeIPPort(const std::string IP, int Port);
std::wstring strToWStr(const std::string orig);
std::string wStrToStr(const std::wstring orig);
class CIPAddress
{
public:
	CIPAddress(const std::string IP="0.0.0.0");
	CIPAddress(const std::array<int, 4> &ip);
	CIPAddress&				setIP(std::string IP);
	char					getClassName();
	std::string				getMaskIP();
	std::string				getAreaName();
	std::string				getDefaultMask();
	std::string				toString();
	std::array<int, 4>		toArray();
	uint32_t				toValue();
	CIPAddress&				operator+(int value);
	void					operator+=(int value);
	CIPAddress&				operator++();
	CIPAddress				operator++(int);
private:
	std::array<int, 4>		IPdiv;
};
class CInterfaceEnum
{
public:
	CInterfaceEnum();
	const std::vector<std::string>&		 getAllInterfaceIP() const;
	std::string							 getHostName() const;
private:
	std::string hostName;
	std::vector<std::string>			 interfaceIP;
};
class CScannerClient
{
public:
	CScannerClient(int servicePort=7890,std::wstring sendMessage=L"Reply Me");
	~CScannerClient();
	CScannerClient&			 setScannerClient(int servicePort=7890, std::wstring sendMessage = L"Reply Me");
	bool					 scan();
	std::string				 getServerIP();
	std::wstring			 wgetServerIP();
	std::string				 getServerMessage();
	std::wstring			 wgetServerMessage();
	int						 getServerPort();
	//void					 showScanningStatus();
private:
	bool					 showStatus;
	bool					 gotPacket;
	std::thread				*scan_thread;
	void					 scan_handler();
	SOCKET					 hostSock;
	SOCKADDR_IN				 hostAddr;
	int						 serverPort;
	std::string				 serverIP;
	std::wstring			 serverMessage;
	std::wstring			 sendMessage;
	std::wstring			 recvMessage;
	CInterfaceEnum			 enumIP;
};
class CScannerServer
{
public:
	CScannerServer(int servicePort=12345,
		std::wstring activeMessage = L"Reply Me",		
		std::wstring replyMessage=L"Hello client!");
	~CScannerServer();
	CScannerServer&		 setActiveMessage(std::wstring newMessage);
	CScannerServer&		 setReplyMessage(std::wstring newMessage);
	bool			     start();
	void				 terminate();
private:
	void				 reply_handler();
	void				 internal_Init();
	SOCKET				 servSock;
	SOCKADDR_IN			 servAddr;
	std::wstring		 activeMessage;
	std::wstring		 replyMessage;
	int					 port;
	std::thread			*reply_thread;
};
class CMulticastExchanger
{
public:
	CMulticastExchanger(std::string multicastIPPort = "224.0.0.10:5555",
		std::string activeMessage = "NoneSetActiveMsg",
		std::string repliedMessage = "NoneSetRepliedMsg");
	~CMulticastExchanger();
	CMulticastExchanger&			setMulticastIPPort(std::string multicastIPPort);
	CMulticastExchanger&			setMulticastIPPort(std::string IP, int port);
	CMulticastExchanger&			setActiveMessage(std::string message,bool subStringMatch = false);
	CMulticastExchanger&			setRepliedMessage(std::string message);
	bool							start();
	bool							sendMessage(std::string message) const;
	bool							sendActiveMessage() const;
	std::string						getReceivedMessage();
	void							terminate();
protected:
	std::string						multicastIP;
	int								multicastPort;
private:
	std::mutex						sync_mutex;
	bool							hasSubStringMatching;
	bool							closed;
	ip_mreq							groupInfo;
	SOCKET							groupSock;
	SOCKADDR_IN						localAddr;
	SOCKADDR_IN						groupAddr;
	std::string						activeMessage;
	std::string						repliedMessage;
	std::string						receivedMessage;
	std::thread					   *receive_thread;
	void							internal_Init();
	void							receive_handle();
};
class CMulticastIPSender : public CMulticastExchanger
{
public:
	CMulticastIPSender(std::string multicastIPPort = "224.0.0.10:5555", int servicePort = 7788);
	void setServicePort(int port);
private:
	void prepareRepliedMessage(std::string multicastIPPort, int servicePort);
	int servicePort;
	CInterfaceEnum ipInterface;
};
class CMulticastIPReceiver : public CMulticastExchanger
{
public:
	CMulticastIPReceiver(std::string multicastIPPort = "224.0.0.10:5555");
	/*
		invoking parseIPPort() to get IP&Port 
	*/
	std::vector<std::string> getAllServerIPPort();
};
class CMulticastChatRoom
{
public:
	CMulticastChatRoom(std::wstring nickname = L"NoName:",std::string multicastIPPort = "224.0.0.11:5556");
	~CMulticastChatRoom();
	CMulticastChatRoom&			setMulticastIPPort(std::string multicastIPPort);
	CMulticastChatRoom&			setMulticastIPPort(std::wstring multicastIPPort);
	CMulticastChatRoom&			setNickname(std::wstring newNickname);
	std::wstring				getFrontMessage();
	bool						sendMessage(std::wstring message);
	bool						join();
	void						terminate();
private:
	std::mutex					sync_lock;
	std::thread*				receive_thread;
	CPacketQueue<std::wstring>	message_queue;
	void					    internal_Init();
	void						receive_handle();
	bool						closed;
	std::wstring				nickname;
	std::string					multicastIP;
	int							multicastPort;
	ip_mreq						groupInfo;
	SOCKET						groupSock;
	SOCKADDR_IN					localAddr;
	SOCKADDR_IN					groupAddr;
};
/*
	This class is implemented to collect 
	fragments that socket send/recv created.
	In order to maintain every packets 
	in complete arrival.
*/
class CFragmentTransfer
{
public:
	CFragmentTransfer();
	int send(int destSock, std::string buf, size_t dataLen);
	int recv(int fromSock, std::string &buf);
private:
};

#endif	