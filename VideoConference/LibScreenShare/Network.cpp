#include "Network.h"
int SocketBase::useCount = 0;
WSADATA SocketBase::wsd;

SocketBase::SocketBase()
{
	if (useCount == 0)
	{
		WSAStartup(MAKEWORD(2, 2), &wsd);
	}
	useCount++;
	sock = 0;
	memset(&sockAddr, 0, sizeof(sockAddr));
	ip.clear();
	port = 0;
	hasError = false;
	lastErrorStr.clear();
}
SocketBase::~SocketBase()
{
	useCount--;
	if (useCount == 0)
	{
		WSACleanup();
		memset(&wsd, 0, sizeof(wsd));
	}
}

bool SocketBase::errorState() const
{
	return hasError;
}

std::string SocketBase::lastError() const
{
	return lastErrorStr;
}

SockServer::SockServer(std::string address)
{
	quit = false;
	acceptThread = nullptr;
	if (!parseIPPort(address, ip, port))
	{
		hasError = true;
		lastErrorStr = "Parse IP/Port format error!";
	}
}
SockServer::~SockServer()
{
	stop();
}

void SockServer::commandProcessor(SOCKET client)
{
	std::string recvBuf;
	cout << "open " << endl;
	CodeBehavior code;
	CPacketer pack;
	CUserData ud;
	CMeetingData md;
	CGroupData_arr gds;
	CMeetingData_arr mds;
	CUserData_arr uds;
	char_arr pwd;
	char_arr title;
	char_arr content;
	int_arr uids;
	int_arr times;
	int deadLine;
	int mid;
	int uid;
	int id;
	int time;
	int count;
	int count2;
	int size1;
	int size2;
	int size3;
	int size4;
	int size5;
	int rtc = 0;
	int result;
	while (!quit)
	{
		if (result = recv(client, recvBuf))
		{
			std::string reply;
			
			
			rtc = 0;

			


			pack.
				Init(recvBuf).
				pull(&code);

			switch (code)
			{
			case C_CHECK_USER:
				pack.pull(&id).
					pull(&pwd, &size1);

				rtc = db.userLogin(id, std::string(pwd.get(), size1));
				if (rtc > 0)
				{
					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						getData();
				}
				break;
			case C_REQUEST_GROUPS:
				rtc = db.getGroups(&gds);
				if (rtc > 0)
				{
					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						insert(gds.get(), rtc, true).
						getData();
				}
				gds.reset();
				break;
			case C_REQUEST_MEETING_MEM_DATA://需求meeting資料
				pack.pull(&id);
				rtc = db.getMeetingMembData(id, &uds);
				if (rtc > 0)
				{
					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						insert(uds.get(), rtc, true).
						getData();
				}
				break;
			case C_REQUEST_USERS_DATA://無資料
				rtc = db.getUsersData(&uds);
				if (rtc > 0)
				{
					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						insert(uds.get(), rtc, true).
						getData();
				}
				break;
			case C_REQUEST_USER_DATA://user id
				pack.pull(&id);
				rtc = db.getUserData(id, &ud);
				if (rtc > 0)
				{
					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						insert(&ud).
						getData();
				}
				break;
			case C_REQUEST_USER_MEETING:// user id
				pack.pull(&id);
				rtc = db.getUserMeeting(id, &mds);
				if (rtc > 0)
				{
					code = C_SUCCESS;
					pack.Init().
						insert(&code).
						insert(&rtc);

					for (int j = 0; j < rtc; j++)
					{
						CMeetingData _md = mds[j];
						pack.insert(&_md.id).
							insert(_md.title.c_str(), _md.title.size(), true).
							insert(_md.content.c_str(), _md.content.size(), true).
							insert(&_md.memAmount).
							insert(&_md.admin).
							insert(&_md.start).
							insert(&_md.end);
					}

					for (int i = 0; i < rtc; i++)
					{
						bool check = db.isVoted(mds[i].id, id);
						if (!check)
						{
							vb.getVoteTimes(mds[i].id, &times, &count, &deadLine);
							if (count != 0)
							{
								pack.insert(&mds[i].id).
									insert(&deadLine).
									insert(times.get(), count, true);
							}
						}
					}
					reply = pack.getData();
				}
				break;
			case C_CREATE_MEETING:
				pack.
					pull(&uid).
					pull(&title, &size1).
					pull(&content, &size2).
					pull(&count).
					pull(&deadLine).
					pull(&uids, &count2).
					pull(&times, &count);
				
				md.admin = uid;
				md.title = std::string(title.get(), size1);
				md.content = std::string(content.get(), size2);
				md.deadLine = deadLine;
				md.memAmount = count2;
				
				rtc = db.addmeeting(md, uids.get());
				
				if (rtc > 0)
				{
					md.id = rtc;
					vb.addNewMeetingVote(md, times.get(), count);

					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						getData();
					
				}
				break;
			case C_UPDATE_USER_NAME://需求user id user name
				break;
			case C_UPDATE_USER_PASSWORD:// 需求 user uid user password
				break;
			case C_SET_VOTED:

				cout << "i voted   i voted   i voted   i voted   i voted" << endl;
				pack.
					pull(&mid).
					pull(&uid).
					pull(&time);

				rtc = db.setVoted(mid, uid);
				if (rtc > 0)
				{
					vb.voteIn(mid, time);

					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						getData();
				}
				break;
			case C_CHCCK_USER_VOTED:
				pack.
					pull(&mid).
					pull(&uid);

				rtc = db.isVoted(mid, uid);
				if (rtc > 0)
				{
					code = C_SUCCESS;
					reply =
						pack.Init().
						insert(&code).
						getData();
				}
				break;
			}


			if (rtc < 1)
			{
				code = C_ERROR;
				reply = pack.Init().
					insert(&code).
					insert(&rtc).
					getData();
			}
			
			if (reply != "" && 
				send(client, reply))
				continue;
			else 
				break;
		}
		else
			break;
	}
	/*
	gds.release();
	mds.release();
	uds.release();
	recvBuf.clear();
	reply.clear();
	pwd.clear();
	pack.Init();
	*/
	//new std::thread(&SockServer::closeProcessor, this, client);
}
void SockServer::closeProcessor(SOCKET client)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);

	std::map<SOCKET, std::thread*>::iterator it;
	it = clientList.find(client);
	if (it != clientList.end())
	{
		shutdown(client, SD_BOTH);
		closesocket(client);
		it->second->join();
		//delete it->second;
		//it->second = nullptr;

		
		//clientList.;
	}
}
CodeBehavior SockServer::send(SOCKET client, std::string data)
{
	if (transfer.send(client, data, data.size()))
		return CodeBehavior::C_SUCCESS;
	else
		return CodeBehavior::C_ERROR;
}
CodeBehavior SockServer::recv(SOCKET client, std::string &recvBuf)
{
	if ((transfer.recv(client, recvBuf)))
			return CodeBehavior::C_SUCCESS;
	else
			return CodeBehavior::C_ERROR;
}
std::string SockServer::getIP() const
{
	return ip;
}
int SockServer::getPort() const
{
	return port;
}
int SockServer::getTotalConnection() const
{
	return clientList.size();
}
SockServer& SockServer::setIP(std::string ip)
{
	this->ip = std::move(ip);
	return *this;
}
SockServer& SockServer::setPort(int port)
{
	this->port = port;
	return *this;
}
bool SockServer::start()
{
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		hasError = true;
		lastErrorStr = "Could not get socket handle from system!";
		return false;
	}
	sockAddr.sin_addr.s_addr = ip == "0.0.0.0" ? INADDR_ANY : inet_addr(ip.c_str());
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	if (::bind(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		hasError = true;
		lastErrorStr = "Could not bind address to server's socket!";
		return false;
	}
	if (listen(sock, 1000) == SOCKET_ERROR)
	{
		closesocket(sock);
		hasError = true;
		lastErrorStr = "Could not enter to listen status!";
		return false;
	}
	quit = false;
	if (!acceptThread)
		acceptThread = new std::thread(&SockServer::accept, this);
	return true;
}
void SockServer::stop()
{
	quit = true;
	if (acceptThread)
	{
		closesocket(sock);
		acceptThread->join();
		delete acceptThread;
		acceptThread = nullptr;
	}
	for (std::map<SOCKET, std::thread*>::iterator client = clientList.begin(); client != clientList.end(); ++client)
	{
		shutdown(client->first, SD_BOTH);
		closesocket(client->first);
		client->second->join();
	}
	clientList.clear();
}

void SockServer::accept()
{
	SOCKET client;
	while (!quit)
	{
		client = ::accept(sock, 0, 0);
		if (client != INVALID_SOCKET)
		{
			clientList[client] = new std::thread(&SockServer::commandProcessor, this, client);
		}
	}
}


SockClient::SockClient(std::string address)
{
	if (!parseIPPort(address, ip, port))
		throw std::logic_error("Parse IP/Port format error!");
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		throw std::runtime_error("Could not get socket handle from system!");
	sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
}
SockClient::~SockClient()
{
	closesocket(sock);
}
SockClient& SockClient::setHost(std::string address)
{
	if (parseIPPort(address, ip, port))
	{
		sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port);
	}
	else
		memset(&sockAddr, 0, sizeof(sockAddr));
	return *this;
}
bool SockClient::connect(const char *newIPPort)
{
	if (newIPPort)
		setHost(newIPPort);
	if (sockAddr.sin_addr.s_addr != 0 &&
		::connect(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
		return false;
	return true;
}
std::string SockClient::sendCommand(std::string data)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);//blocking, make other command wait.
	std::string recvBuf;
		
	if (send(data))
	{
		if (recv(recvBuf))
			return recvBuf;
	}
	return "";
}
CodeBehavior SockClient::send(std::string data)
{
	if (transfer.send(sock, data, data.size()))
		return CodeBehavior::C_SUCCESS;
	else
		return CodeBehavior::C_ERROR;
}
CodeBehavior SockClient::recv(std::string &recvBuf)
{
		if (transfer.recv(sock, recvBuf))
			return CodeBehavior::C_SUCCESS;
		else
			return CodeBehavior::C_ERROR;
}