#include "NetworkUtils.h"

bool parseIPPort(const std::string IPPort, std::string &IPAddress, int &Port)
{
	std::stringstream tokss , ss;
	std::string out;
	int cnts = 0 , val;
	tokss << IPPort;
	while (std::getline(tokss, out, '.'))
	{
		ss << out;
		ss >> val;
		if (val > 255 || val < 0) //is not IPv4
			return false;
		cnts++;
		ss.clear();
	}
	if (cnts != 4) //is not IPv4
		return false;
	auto k = IPPort.find(":");
	if (k == std::string::npos) //missing :
		return false;
	out.assign(IPPort.begin() + k + 1, IPPort.begin() + IPPort.size());
	if (!out.size()) //missing Port number
		return false;
	ss.str("");
	ss.clear();
	ss << out;
	ss >> val;
	if (val > 65535 || val < 0) //out of port number range
		return false;
	IPAddress.assign(IPPort.begin(), IPPort.begin() + k);
	Port = val;
	return true;
}
std::wstring makeIPPort(const std::wstring IP, int Port)
{
	std::wstringstream ss;
	ss << IP << L":" << Port;
	return ss.str();
}
std::string makeIPPort(const std::string IP, int Port)
{
	std::stringstream ss;
	ss << IP << ":" << Port;
	return ss.str(); 
}
std::wstring strToWStr(const std::string orig)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return convertor.from_bytes(orig);
}
std::string wStrToStr(const std::wstring orig)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return convertor.to_bytes(orig);
}
CIPAddress::CIPAddress(const std::string IP)
{
	setIP(IP);
}
CIPAddress::CIPAddress(const std::array<int, 4> &ip)
{
	IPdiv = ip;
}
CIPAddress& CIPAddress::setIP(std::string IP)
{
	std::stringstream tokss, ss;
	std::string out;
	int cnts = 0, val;
	tokss << IP;
	IPdiv[0] = IPdiv[1] = IPdiv[2] = IPdiv[3] = -1;
	while (std::getline(tokss, out, '.'))
	{
		ss << out;
		ss >> val;
		if (val > 255 || val < 0) //is not IPv4
		{
			IPdiv[0] = IPdiv[1] = IPdiv[2] = IPdiv[3] = -1;
			break;
		}
		IPdiv[cnts++] = val;
		ss.clear();
	}
	return *this;
}
char CIPAddress::getClassName()
{
	if (IPdiv[0] == -1)
		return '\0';
	if (IPdiv[0] >= 1 && IPdiv[0] <= 126 && IPdiv[0] != 10)
		return 'A';
	else if (IPdiv[0] >= 128 && IPdiv[0] <= 191 && !(IPdiv[0] == 172 && IPdiv[1] >= 16 && IPdiv[1] <= 31))
		return 'B';
	else if (IPdiv[0] >= 192 && IPdiv[0] <= 223 && IPdiv[1] != 168)
		return 'C';
	else if (IPdiv[0] == 127 && IPdiv[1] == 0 && IPdiv[2] == 0 && IPdiv[3] == 1)
		return 'L';
	else
		return 'P'; //Private IP
}
std::string	CIPAddress::getMaskIP()
{
	std::string mask = getDefaultMask();
	if (mask != "Error")
	{
		const auto &arr1 = IPdiv;
		auto arr2 = CIPAddress(mask).toArray();
		for (int i = 0; i < 4; i++)
		{
			arr2[i] = arr1[i] & arr2[i];
		}
		return CIPAddress(arr2).toString();
	}
	return "Error";
}
std::string CIPAddress::getAreaName()
{
	char ret;
	if ((ret = getClassName()) == 'P')
		return "private";
	else if (ret == 'L')
		return "localhost";
	else if (ret == '\0')
		return "error";
	else
		return "public";
}
std::string CIPAddress::getDefaultMask()
{
	switch (getClassName())
	{
	case 'A':
		return "255.0.0.0";
	case 'B':
		return "255.255.0.0";
	case 'C':
		return "255.255.255.0";
	case 'P':
		if (IPdiv[0] == 10)
			return "255.0.0.0";
		else if (IPdiv[0] == 172)
			return "255.240.0.0";
		else if (IPdiv[0] == 192)
			return "255.255.255.0";
		else
			return "Error";
		break;
	default:
		return "Error";
	}
}
std::string CIPAddress::toString()
{
	std::stringstream ss;
	for (int i = 0; i < 4; i++)
	{
		ss << IPdiv[i];
		if (i < 3)
			ss << ".";
	}
	return ss.str();
}
uint32_t CIPAddress::toValue()
{
	uint32_t val = 0;
	uint8_t *ptr = (uint8_t*)&val;
	int cnt = 3;
	for (int i = 0; i < 4; i++)
		ptr[cnt--] = IPdiv[i];
	return val;
}
std::array<int, 4> CIPAddress::toArray()
{
	return IPdiv;
}
CIPAddress& CIPAddress::operator+(int value)
{
	uint32_t val = 0;
	uint8_t *ptr = (uint8_t*)&val;
	int cnt = 3;
	for (int i = 0; i < 4; i++)
		ptr[cnt--] = IPdiv[i];
	val += value;
	cnt = 0;
	for (int i = 3; i >= 0; i--)
		IPdiv[i] = ptr[cnt++];
	return *this;
}
void CIPAddress::operator+=(int value)
{
	operator+(value);
}
CIPAddress& CIPAddress::operator++()
{
	return operator+(1);
}
CIPAddress CIPAddress::operator++(int)
{
	CIPAddress temp(*this);
	++(*this);
	return temp;
}
CInterfaceEnum::CInterfaceEnum()
{
	char buf[1024];
	if (gethostname(buf, sizeof(buf)) == SOCKET_ERROR)
	{
		puts("CInterfaceEnum:Could not get hostname, perhaps forgot initializing socket libraries?");
		return;
	}
	hostName.assign(buf);

	struct hostent *phe = gethostbyname(buf);

	if (phe == 0) 
	{
		puts("CInterfaceEnum:Could not get hostent data, perhaps forgot initializing socket libraries?");
		return;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) 
	{
		interfaceIP.push_back(inet_ntoa(*((struct in_addr*)phe->h_addr_list[i])));
	}

}
const std::vector<std::string> &CInterfaceEnum::getAllInterfaceIP() const
{
	return interfaceIP;
}
std::string CInterfaceEnum::getHostName() const
{
	return hostName;
}	
CScannerClient::CScannerClient(int servicePort, std::wstring sendMessage)
{
	setScannerClient(servicePort, sendMessage);
}
CScannerClient::~CScannerClient()
{
	gotPacket = true;
}
CScannerClient&	CScannerClient::setScannerClient(int servicePort, std::wstring sendMessage)
{
	if (scan_thread)
	{
		gotPacket = true;
		scan_thread->join();
		delete scan_thread;
	}
	serverIP.clear();
	serverMessage.clear();
	gotPacket = false;
	scan_thread = nullptr;
	serverPort = servicePort;
	this->sendMessage.assign(sendMessage);
	return *this;
}
bool CScannerClient::scan()
{
	char buf[1024]; 
	wchar_t *ptr = (wchar_t*)buf;
	int ret;
	SOCKADDR_IN sAddr;
	int size = sizeof(sAddr);
	if (!scan_thread)
	{
		hostSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		hostAddr.sin_family = AF_INET;
		hostAddr.sin_port = htons(serverPort);
		scan_thread = new std::thread(&CScannerClient::scan_handler, this);
		do
		{
			ret = recvfrom(hostSock, buf, sizeof(buf), 0, (SOCKADDR*)&sAddr, &size);
			Sleep(10);
		} while (!gotPacket && ret < 0);
		gotPacket = true;
		scan_thread->join();
		delete scan_thread;
		scan_thread = nullptr;
		serverIP.assign(inet_ntoa(sAddr.sin_addr));
		serverMessage.assign(ptr);
		return true;
	}
	else
	{
		closesocket(hostSock);
		gotPacket = true;
		scan_thread->join();
		delete scan_thread;
		scan_thread = nullptr;
		serverMessage.clear();
		return false;
	}	
}
std::string	CScannerClient::getServerIP()
{
	return serverIP;
}
std::wstring CScannerClient::wgetServerIP()
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return convertor.from_bytes(serverIP);
}
std::string CScannerClient::getServerMessage()
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return convertor.to_bytes(serverMessage);
}
std::wstring CScannerClient::wgetServerMessage()
{
	return serverMessage;
}
int	CScannerClient::getServerPort()
{
	return serverPort;
}
void CScannerClient::scan_handler()
{
	int outSize;
	auto iplist = enumIP.getAllInterfaceIP();
	char buf[1024];
	char startClass;
	CIPAddress interfaceIP, scanningIP;
	memcpy(buf, sendMessage.c_str(), outSize = sizeof(wchar_t) * (sendMessage.size() + 1));
	for (const auto &ip : iplist)
	{
		interfaceIP.setIP(ip);
		scanningIP = interfaceIP.getMaskIP();
		startClass = scanningIP.getClassName();
		do
		{
			hostAddr.sin_addr.s_addr = inet_addr(scanningIP.toString().c_str());
			sendto(hostSock, buf, outSize, 0, (SOCKADDR*)&hostAddr, sizeof(hostAddr));
			puts(scanningIP.toString().c_str());
		} while (!gotPacket && (++scanningIP).getClassName() == startClass);
	}
}
CScannerServer::CScannerServer(int servicePort,std::wstring activeMessage,std::wstring replyMessage)
{
	internal_Init();
	port = servicePort;
	this->activeMessage = activeMessage;
	this->replyMessage = replyMessage;
}
CScannerServer::~CScannerServer()
{
	terminate();
}
bool CScannerServer::start()
{
	servSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (servSock == INVALID_SOCKET)
	{
		puts("CScannerServer:Could not get socket handle.");
		return false;
	}
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr))
		== SOCKET_ERROR)
	{
		puts("CScannerServer:Could not bind socket data.");
		return false;
	}
	reply_thread = new std::thread(&CScannerServer::reply_handler, this);
	return true;
}
CScannerServer& CScannerServer::setActiveMessage(std::wstring newMessage)
{
	activeMessage.assign(newMessage);
	return *this;
}
CScannerServer& CScannerServer::setReplyMessage(std::wstring newMessage)
{
	replyMessage.assign(newMessage);
	return *this;
}
void CScannerServer::terminate()
{
	closesocket(servSock);
	if (reply_thread)
	{
		reply_thread->join();
		delete reply_thread;
		reply_thread = NULL;
	}
}
void CScannerServer::reply_handler()
{
	SOCKADDR_IN sAddr;
	char buf[1024];
	char out[1024];
	int outSize;
	wchar_t *ptr=(wchar_t*)buf;
	int size,ret;
	memcpy(out, replyMessage.c_str(), (outSize = sizeof(wchar_t) * (replyMessage.size() + 1)));
	while (1)
	{
		size = sizeof(sAddr);
		ret = recvfrom(servSock, buf, 1024, 0, (SOCKADDR*)&sAddr, &size);
		if (ret <= 0)
			break;
		else if (!wcscmp(ptr, activeMessage.c_str()))
		{
			sendto(servSock, out, outSize, 0, (SOCKADDR*)&sAddr, size);
		}
	}
}
void CScannerServer::internal_Init()
{
	reply_thread = nullptr;
	memset(&servAddr, 0, sizeof(servAddr));
	activeMessage.clear();
	replyMessage.clear();
	servSock = 0;
	port = 0;
}
CMulticastExchanger::CMulticastExchanger(std::string multicastIPPort
	, std::string activeMessage,
	std::string repliedMessage)
{
	internal_Init();
	setMulticastIPPort(multicastIPPort);
	setActiveMessage(activeMessage);
	setRepliedMessage(repliedMessage);
}
CMulticastExchanger::~CMulticastExchanger()
{
	terminate();
}
CMulticastExchanger& CMulticastExchanger::setMulticastIPPort(std::string multicastIPPort)
{
	if (!parseIPPort(multicastIPPort, multicastIP, multicastPort))
	{
		printf("CMulticastExchanger:Parsing [%s] failed, is the format really correct?\n",
			multicastIPPort.c_str());
	}
	return *this;
}
CMulticastExchanger& CMulticastExchanger::setMulticastIPPort(std::string IP, int port)
{
	multicastIP.assign(IP);
	multicastPort = port;
	return *this;
}
CMulticastExchanger& CMulticastExchanger::setActiveMessage(std::string message, bool subStringMatch)
{
	activeMessage.assign(message);
	hasSubStringMatching = subStringMatch;
	return *this;
}
CMulticastExchanger& CMulticastExchanger::setRepliedMessage(std::string message)
{
	if (message != activeMessage)
		repliedMessage.assign(message);
	else
	{
		printf("CMulticastExchanger:Do not set replied & active to the same.");
		repliedMessage.assign("Do not set replied & active to the same!");
	}
	return *this;
}
bool CMulticastExchanger::start()
{
	if (!receive_thread)
	{
		int reuse;
		char loopch = 0;
		closed = false;
		groupSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (groupSock == INVALID_SOCKET)
			return false;		

		if (setsockopt(groupSock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
		{
			printf("CMulticastExchanger:Setting SO_REUSEADDR error");
			closesocket(groupSock);
			return false;
		}
		
		localAddr.sin_family = AF_INET;
		localAddr.sin_port = htons(multicastPort);
		localAddr.sin_addr.s_addr = INADDR_ANY;
		
		if (bind(groupSock, (SOCKADDR*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
		{
			printf("CMulticastExchanger:Binding datagram socket error:%d", GetLastError());
			closesocket(groupSock);
			return false;
		}

		groupInfo.imr_multiaddr.s_addr = inet_addr(multicastIP.c_str());
		groupInfo.imr_interface.s_addr = INADDR_ANY;

		groupAddr.sin_addr.s_addr = groupInfo.imr_multiaddr.s_addr;
		groupAddr.sin_family = AF_INET;
		groupAddr.sin_port = htons(multicastPort);

		if (setsockopt(groupSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&groupInfo, sizeof(groupInfo)) < 0)
		{
			printf("CMulticastExchanger:Adding multicast group error:%d", GetLastError());
			closesocket(groupSock);
			return false;
		}

#ifndef _DEBUG		
		if (setsockopt(groupSock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
		{
			perror("Setting IP_MULTICAST_LOOP error");
			closesocket(groupSock);
			exit(1);
		}
#endif

		printf("CMulticastExchanger has joined to Multicast [%s:%d]\n", multicastIP.c_str(), multicastPort);
		receive_thread = new std::thread(&CMulticastExchanger::receive_handle, this);
		return true;
	}
	return false;
}
bool CMulticastExchanger::sendMessage(std::string message) const
{
	if (sendto(groupSock, message.c_str(), message.size() + 1,
		0, (SOCKADDR*)&groupAddr, sizeof(groupAddr)) < 0)
		return false;
	return true;
}
bool CMulticastExchanger::sendActiveMessage() const
{
	return sendMessage(activeMessage);
}
std::string CMulticastExchanger::getReceivedMessage()
{
	std::unique_lock<std::mutex> unique_lck(sync_mutex);
	return receivedMessage;
}
void CMulticastExchanger::terminate()
{
	if (receive_thread)
	{
		closed = true;
		setsockopt(groupSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&groupInfo, sizeof(groupInfo));
		closesocket(groupSock);
		receive_thread->join();
		delete receive_thread;
		receive_thread = nullptr;
	}
}
void CMulticastExchanger::internal_Init()
{
	multicastIP.clear();
	multicastIP.clear();
	activeMessage.clear();
	repliedMessage.clear();
	receivedMessage.clear();
	memset(&groupInfo, 0, sizeof(groupInfo));
	memset(&localAddr, 0, sizeof(localAddr));
	groupAddr = localAddr;
	groupSock = 0;
	multicastPort = 0;
	closed = false;
	hasSubStringMatching = false;
	receive_thread = nullptr;
}
void CMulticastExchanger::receive_handle()
{
	int bytes;
	char buf[10240];
	std::string str;
	while (!closed)
	{
		bytes = recv(groupSock, buf, sizeof(buf), 0);
		if (bytes > 0)
		{
			sync_mutex.lock();
			if (!hasSubStringMatching && std::string(buf) == activeMessage)
			{
				receivedMessage.assign(buf);
				sendMessage(repliedMessage);
			}
			else if (hasSubStringMatching && std::string(buf).find(activeMessage) != std::string::npos)
			{
				receivedMessage.assign(buf);
				sendMessage(repliedMessage);
			}
			sync_mutex.unlock();
		}
	}
}
CMulticastIPSender::CMulticastIPSender(std::string multicastIPPort, int servicePort)
{
	this->servicePort = servicePort;
	prepareRepliedMessage(multicastIPPort, servicePort);
}
void CMulticastIPSender::setServicePort(int port)
{
	this->servicePort = port;
	prepareRepliedMessage(makeIPPort(multicastIP, multicastPort), port);
}
void CMulticastIPSender::prepareRepliedMessage(std::string multicastIPPort, int servicePort)
{
	auto v = ipInterface.getAllInterfaceIP();
	std::string repliedMessage = "Reply to you IP/Port\n";
	if (v.size() <= 0)
	{
		puts("CMulticastIPSender:Could not get any interface IP from this computer.");
		return;
	}
	for (const auto &i : v)
	{
		repliedMessage += i + ":";
		repliedMessage +=
			[&](int val) -> std::string
			{
				std::stringstream ss;
				std::string out;
				ss << val;
				ss >> out;				
				return ss.str();
			}(servicePort); //int to string (with lambda function)
		repliedMessage += "\n";
	}
	//remove last '\n' from repliedMessage
	repliedMessage[repliedMessage.size() - 1] = 0;
	repliedMessage.resize(repliedMessage.size() - 1);
	setMulticastIPPort(multicastIPPort);
	setActiveMessage("Give me IP/Port");
	setRepliedMessage(repliedMessage);
}
CMulticastIPReceiver::CMulticastIPReceiver(std::string multicastIPPort)
{
	setMulticastIPPort(multicastIPPort);
	setActiveMessage("Reply to you IP/Port",true);
	setRepliedMessage("Got IP/Port");
}
std::vector<std::string> CMulticastIPReceiver::getAllServerIPPort()
{	
	std::string s;
	unsigned int pos;
	std::vector<std::string> out;
	sendMessage("Give me IP/Port");
	Sleep(200);
	s = getReceivedMessage();
	pos = s.find("Reply to you IP/Port\n");
	if (pos != std::string::npos)
	{
		std::stringstream ss;
		std::string temp;
		std::string ip;
		pos += strlen("Reply to you IP/Port\n"); //'\n'
		s = s.substr(pos);
		ss << s;
		while (getline(ss, temp, '\n'))
			out.push_back(temp);
	}
	return out;
}
CMulticastChatRoom::CMulticastChatRoom(std::wstring nickname, std::string multicastIPPort)
{
	internal_Init();
	setMulticastIPPort(multicastIPPort);
	setNickname(nickname);
}
CMulticastChatRoom::~CMulticastChatRoom()
{
	terminate();
}
CMulticastChatRoom& CMulticastChatRoom::setMulticastIPPort(std::string multicastIPPort)
{
	if (!parseIPPort(multicastIPPort, multicastIP, multicastPort))
	{
		printf("CMulticastChatRoom:Parsing [%s] failed, is the format really correct?\n",
			multicastIPPort.c_str());
	}
	return *this;
}
CMulticastChatRoom& CMulticastChatRoom::setMulticastIPPort(std::wstring multicastIPPort)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	if (!parseIPPort(convertor.to_bytes(multicastIPPort), multicastIP, multicastPort))
	{
		printf("CMulticastChatRoom:Parsing [%s] failed, is the format really correct?\n",
			multicastIPPort.c_str());
	}
	return *this;
}
CMulticastChatRoom& CMulticastChatRoom::setNickname(std::wstring newNickname)
{
	nickname = newNickname;
	return *this;
}
std::wstring CMulticastChatRoom::getFrontMessage()
{
	auto list = message_queue.getItemNonBlocking();
	auto out = std::wstring(L"");
	if (list)
	{
		out = list->data;
		delete list;
		return out;
	}
	return out;
}
bool CMulticastChatRoom::sendMessage(std::wstring message)
{
	int ret;
	char buf[1500];
	if (message.size() > 749)
		return false;
	message = nickname + L"：" + message;
	memcpy(buf, message.c_str(), ret = sizeof(wchar_t) * (message.size() + 1));	
	if (sendto(groupSock, buf, ret,
		0, (SOCKADDR*)&groupAddr, sizeof(groupAddr)) < 0)
		return false;
	return true;
}
bool CMulticastChatRoom::join()
{
	if (!receive_thread)
	{
		int reuse;
		char loopch = 0;
		groupSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (groupSock == INVALID_SOCKET)
			return false;

		if (setsockopt(groupSock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
		{
			printf("CMulticastChatRoom:Setting SO_REUSEADDR error");
			closesocket(groupSock);
			return false;
		}

		localAddr.sin_family = AF_INET;
		localAddr.sin_port = htons(multicastPort);
		localAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(groupSock, (SOCKADDR*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
		{
			printf("CMulticastChatRoom:Binding datagram socket error:%d", GetLastError());
			closesocket(groupSock);
			return false;
		}

		groupInfo.imr_multiaddr.s_addr = inet_addr(multicastIP.c_str());
		groupInfo.imr_interface.s_addr = INADDR_ANY;

		groupAddr.sin_addr.s_addr = groupInfo.imr_multiaddr.s_addr;
		groupAddr.sin_family = AF_INET;
		groupAddr.sin_port = htons(multicastPort);

		if (setsockopt(groupSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&groupInfo, sizeof(groupInfo)) < 0)
		{
			printf("CMulticastChatRoom:Adding multicast group error:%d", GetLastError());
			closesocket(groupSock);
			return false;
		}

		/*
		if (setsockopt(groupSock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
		{
		perror("Setting IP_MULTICAST_LOOP error");
		closesocket(groupSock);
		exit(1);
		}
		*/

		printf("CMulticastChatRoom has joined to Multicast [%s:%d]\n", multicastIP.c_str(), multicastPort);
		receive_thread = new std::thread(&CMulticastChatRoom::receive_handle, this);
		return true;
	}
	return false;
}
void CMulticastChatRoom::terminate()
{
	if (receive_thread)
	{
		closed = true;
		setsockopt(groupSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&groupInfo, sizeof(groupInfo));
		closesocket(groupSock);
		receive_thread->join();
		delete receive_thread;
		receive_thread = nullptr;
	}
}
void CMulticastChatRoom::internal_Init()
{
	memset(&groupInfo, 0, sizeof(groupInfo));
	memset(&localAddr,0,sizeof(localAddr));
	groupSock = 0;
	groupAddr = localAddr;
	multicastIP.clear();
	multicastPort = 0;
	message_queue.setMaxSize(20);
	closed = false;
	receive_thread = nullptr;
	nickname.clear();
}
void CMulticastChatRoom::receive_handle()
{
	int bytes;
	char buf[10000];
	wchar_t *ptr = (wchar_t*)buf;
	closed = false;
	while (!closed)
	{
		bytes = recv(groupSock, buf, sizeof(buf), 0);
		if (bytes > 0)
		{
			message_queue.pushNonBlocking(ptr);
		}
	}
}
CFragmentTransfer::CFragmentTransfer()
{
}
int CFragmentTransfer::send(int destSock, std::string buf, size_t dataLen)
{
	std::string sendBuf;

	int packet, size, ret;
	union{ char bytes[4]; uint32_t value; } u;
	uint32_t nlen = htonl(dataLen);

	if (sendBuf.max_size() < sizeof(uint32_t)+dataLen)
		throw std::runtime_error("No enough memories to allocate.");

	u.value = nlen;
	sendBuf += std::string(u.bytes, 4);
	sendBuf += buf;
	size = sizeof(uint32_t) + dataLen;
	packet = 0;
	while (packet < size)
	{
		ret = ::send(destSock, sendBuf.c_str() + packet, size - packet, 0);
		if (ret <= 0)
			return 0;
		packet += ret;
	}
	return size;
}
int CFragmentTransfer::recv(int fromSock, std::string &buf)
{
	char *recvBuf;
	char sizeBuf[12];
	uint32_t size, packet;

	int ret = ::recv(fromSock, sizeBuf, 4, 0);

	//printf("%d", WSAGetLastError());

	if (ret < 4)
		return 0;

	memcpy(&size, sizeBuf, sizeof(uint32_t));

	size = ntohl(size);
	recvBuf = new char[size];
	packet = 0;
	while (packet < size)
	{
		ret = ::recv(fromSock, recvBuf + packet, size - packet, 0);
		if (ret <= 0)
			return 0;
		packet += ret;
	}

	buf = std::string(recvBuf, size);
	delete recvBuf;
	return size;
}
