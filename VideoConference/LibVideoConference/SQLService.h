#ifndef SQLSERVICE_H
#define SQLSERVICE_H
#include "sqlite3.h"
#include <iostream>
#include <string> 
#include <ctime>
#include <memory>
#include <mutex>
#include <map>
#include <set>

/*
Basic result code
*/
#define SQL_ERROR								-1
#define SQL_FAIL								0
#define SQL_SUCCESS								1
/*
Specific result code 
*/
#define SQL_LOGIN_USER_NOT_EXIST				-100
#define SQL_REGISTER_USER_EXIST					-101
#define SQL_REGISTER_GROUP_NOT_EXIST			-102
#define SQL_ADDGROUP_GROUP_EXIST				-103
#define SQL_MEETING_USER_NOT_EXIST				-104
#define SQL_UPDATE_GROUP_EXIST					-105
#define SQL_UPDATE_GROUP_NOT_EXIST				-106
#define SQL_UPDATE_INCORRECT_CREATOR			-107

#define USER_ID									30
#define MEETING_ID								31

#define ROW_MEETING								300
#define ROW_USER								301
#define ROW_GROUP								302
#define ROW_MEETING_MEM_DATA					303
using namespace std;				
struct CMeetingData
{
	int				id;
	std::string		title;
	std::string		content;
	int				deadLine;
	int				memAmount;
	int 			start;
	int				end;
	int				admin;
};

struct CUserData
{
	int				id;
	std::string		name;
	std::string		pwd;
	int				groupid;
	int				isCreator;
	int				lastLogin;
	int				lastLogout;
	int				registerTime;

};
struct CGroupData
{
	int				id;
	std::string		name;
};
typedef std::unique_ptr<CMeetingData[]>			CMeetingData_arr;
typedef std::unique_ptr<CUserData[]>			CUserData_arr;
typedef std::unique_ptr<CGroupData[]>			CGroupData_arr;
typedef std::unique_ptr<int[]>					int_arr;
typedef std::unique_ptr<char[]>					char_arr;
enum TimeType
{
	START,
	END,
};


class CSQLService
{
public:

	CSQLService();
	~CSQLService();
	
	/*
		Input user's account and password entered.
	*/
	int						userLogin			(_In_ int uid,			_Out_ std::string pwd);

	/*
		If you want to use that, you need use the following
		1.name 
		2.pwd
		3.groupid
		4.isCreator
		from struct "CUserData".
	*/
	int					addUser				(_In_ CUserData ud);
	int					addmeeting			(_In_ CMeetingData md,	_In_ int *ud);//缺少file/////v
	int					addGroup			(_In_ std::string gn);
	int					deleteGroup			(_In_ int gid);
	int					getGroups			(_Out_ CGroupData_arr *gds);/////v
	int					getUserMeeting(_In_ int userid, _Out_ CMeetingData_arr *mds);/////v
	int					getUserData			(_In_ int userid,		_Out_ CUserData *ud);/////v
	int					getUsersData		(_Out_ CUserData_arr *uds);/////v
	int					getMeetingMembData	(_In_ int mid,			_Out_ CUserData_arr *uds);/////v
	int					updateGroupData		(_In_ int gid,			_In_ std::string name);
	int					updateUserName		(_In_ int uid,			_In_ std::string name);/////v
	int					updateUserGroup		(_In_ int uid,			_In_ int group);
	int					updateUserCreator	(_In_ int uid,			_In_ int isCreator);
	int					updateUserPwd		(_In_ int uid,			_In_ std::string pwd);/////v
	int					updateMeetingTime	(_In_ int mid,			_In_ int time, TimeType tt);
	
	int					setVoted(int mid, int uid);
	int					isVoted(int mid, int uid);
private:

	bool				internal_init();
	void				database_Init();
	void				mutex_LOCK			(_In_ sqlite3* _db);
	void				mutex_UNLOCK		(_In_ sqlite3* _db);
	int					getRow(_In_ int table);
	int					getRow(_In_ std::string countSql);
	int					insertMember		(_In_ int mid,			_In_ int memAmount,		_In_ int *ud);
	int					hasUser				(_In_ int uid);
	int					hasGroup			(_In_ int gid);
	char				*dbName;
	static int			useCount;
	static std::mutex	sync_mtx;
	sqlite3		*db;

};
#endif