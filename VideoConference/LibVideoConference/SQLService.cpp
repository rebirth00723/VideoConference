#include <SQLService.h>
int CSQLService::useCount = 0;
std::mutex CSQLService::sync_mtx;
static sqlite3 CSQLService::*db = nullptr;
CSQLService::~CSQLService()
{
	--useCount;
	if (useCount == 0)
	{
		sqlite3_close(db);
		db = nullptr;
		dbName = nullptr;
	}
}

CSQLService::CSQLService()
{
	bool okay;
	okay = internal_init();
	if (useCount)
		useCount++;
	else
	{
		if (okay)
		{
			useCount++;
			database_Init();
		}
	}
}
bool CSQLService::internal_init()
{
	dbName = "server.db";
	int rc;
	rc = sqlite3_open(dbName, &db);
	if (rc)
	{
		std::fprintf(stderr, "Can't open database: %s.\n", sqlite3_errmsg(db));
		return false;
	}
	else
	{
		sqlite3_exec(db, "PRAGMA foreign_keys = on", NULL, NULL, NULL);
		std::fprintf(stdout, "Opened database successfully.\n");
		return true;
	}
}
void CSQLService::database_Init()
{
	int rc;
	char *sql;

	
	/*
	TABLE user
	*/
	sql =
		"CREATE TABLE IF NOT EXISTS `user` ("
		"`id`			INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
		"`name`			TEXT NOT NULL UNIQUE,"
	  //"`account`		TEXT NOT NULL UNIQUE,"
		"`password`		TEXT NOT NULL,"
		"`groupid`		INTEGER NOT NULL DEFAULT 1,"
		"`creator`		INTEGER NOT NULL DEFAULT 0,"
		"`lastlogin`	INTEGER DEFAULT 0,"
		"`lastlogout`	INTEGER DEFAULT 0,"
		"`register`		INTEGER DEFAULT 0,"
		"FOREIGN KEY(`groupid`) REFERENCES `group`(`id`) ON DELETE SET DEFAULT"
		");";
	rc = sqlite3_exec(db, sql, NULL, 0, NULL);
	if (rc != SQLITE_OK)
		std::fprintf(stderr, "SQL error: %s.\n", sqlite3_errmsg(db));
	else
		std::fprintf(stderr, "Created table `user` successfully.\n");

	/*
	TABLE group
	*/
	sql =
		"CREATE TABLE IF NOT EXISTS `group` ("
		"`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
		"`name`	INTEGER NOT NULL UNIQUE"
		");";
	rc = sqlite3_exec(db, sql, NULL, 0, NULL);
	if (rc != SQLITE_OK)
		std::fprintf(stderr, "SQL error: %s.\n", sqlite3_errmsg(db));
	else
		std::fprintf(stderr, "Created table `group` successfully.\n");


	/*
	TABLE meeting
	*/
	sql =
		"CREATE TABLE IF NOT EXISTS `meeting` ("
		"`id`		INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
		"`title`	TEXT DEFAULT none,"
		"`content`	TEXT DEFAULT none,"
		"`memAmount` INTEGER NOT NULL DEFAULT 1,"
	//	"`state`	INTEGER NOT NULL DEFAULT 0,"
	//	"`deadLine`	INTEGER NOT NULL DEFAULT 63112320,"
		"`start`	INTEGER NOT NULL DEFAULT 0,"
		"`end`		INTEGER NOT NULL DEFAULT 0,"
		"`admin`	INTEGER NOT NULL DEFAULT 1,"
		"FOREIGN KEY(`admin`) REFERENCES `user`(`id`)"
		");";
	rc = sqlite3_exec(db, sql, NULL, 0, NULL);
	if (rc != SQLITE_OK)
		std::fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
	else
		std::fprintf(stderr, "Created table `meeting` successfully.\n");

	/*
	TABLE meetingmember
	*/
	sql =
		"CREATE TABLE IF NOT EXISTS `meetingmember` ("
		"`meetingid`	INTEGER NOT NULL,"
		"`userid`		INTEGER NOT NULL,"
		"`isVoted`		INTEGER NOT NULL DEFAULT 0,"
		"FOREIGN KEY(`userid`) REFERENCES `user`(`id`) ON DELETE CASCADE,"
		"PRIMARY KEY(`meetingid`,`userid`),"
		"FOREIGN KEY(`meetingid`) REFERENCES `meeting`(`id`) ON DELETE CASCADE"
		");";
	rc = sqlite3_exec(db, sql, NULL, 0, NULL);
	if (rc != SQLITE_OK)
		std::fprintf(stderr, "SQL error: %s.\n", sqlite3_errmsg(db));
	else
		std::fprintf(stderr, "Created table `meetingmember` successfully.\n");
	
	/*
	TABLE meetingtime
	*/
	/*sql =
		"CREATE TABLE IF NOT EXISTS `meetingtime` ("
		"`meetingid`	INTEGER NOT NULL,"
		"`time`	INTEGER NOT NULL,"
		"FOREIGN KEY(`meetingid`) REFERENCES `meeting`(`id`)"
		");";
	rc = sqlite3_exec(db, sql, NULL, 0, NULL);
	if (rc != SQLITE_OK)
		std::fprintf(stderr, "SQL error: %s.\n", sqlite3_errmsg(db));
	else
		std::fprintf(stderr, "Created table `meetingtime` successfully.\n");
		*/
	/*
	TABLE timevote
	*/
	/*sql =
		"CREATE TABLE IF NOT EXISTS `timevote` ("
		"`meetingid`	INTEGER NOT NULL,"
		"`userid`	INTEGER NOT NULL,"
		"`time`	INTEGER NOT NULL,"
		"`count`	INTEGER NOT NULL DEFAULT 0,"
		"FOREIGN KEY(`meetingid`) REFERENCES `meeting`(`id`),"
		"PRIMARY KEY(`meetingid`,`userid`),"
		"FOREIGN KEY(`userid`) REFERENCES `user`(`id`)"
		");";
	rc = sqlite3_exec(db, sql, NULL, 0, NULL);
	if (rc != SQLITE_OK)
		std::fprintf(stderr, "SQL error: %s.\n", sqlite3_errmsg(db));
	else
		std::fprintf(stderr, "Created table `timevote` successfully.\n");
	*/
	/*
	Basic Value initial
	*/
	this->addGroup("default");
	
}
void CSQLService::mutex_LOCK(sqlite3* _db)
{
	sqlite3_mutex_enter(sqlite3_db_mutex(_db));
}
void CSQLService::mutex_UNLOCK(sqlite3* _db)
{
	sqlite3_mutex_leave(sqlite3_db_mutex(_db));
}

int CSQLService::hasUser(int uid)
{
	std::string sql;
	int rtc = SQL_ERROR;
	sqlite3_stmt* stmt = NULL;

	sql = 
		"SELECT `id` "
		"FROM `user` "
		"WHERE (`id` = ?)"
		";";
	
	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

	sqlite3_bind_int(stmt, 1, uid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		rtc = SQL_SUCCESS;
		break;
	case SQLITE_DONE:
		rtc = SQL_FAIL;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'hasUser'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::hasGroup(int gid)
{
	std::string sql;
	int rtc = SQL_ERROR;
	sqlite3_stmt* stmt = NULL;

	sql =
		"SELECT `id` "
		"FROM `group` "
		"WHERE (`id` = ?)"
		";";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

	sqlite3_bind_int(stmt, 1, gid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		rtc = SQL_SUCCESS;
		break;
	case SQLITE_DONE:
		rtc = SQL_FAIL;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'hasGroup'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}

int CSQLService::userLogin(int uid, std::string pwd)
{
	char* sql;
	sqlite3_stmt* stmt = NULL;
	int rtc = SQL_ERROR;

	std::fprintf(stdout, "User(%d) logining...\n", uid);

	switch (hasUser(uid))
	{
	case SQL_FAIL:
		std::fprintf(stdout, "User(%d) is not exist.\n", uid);
		return SQL_LOGIN_USER_NOT_EXIST;
	case SQL_ERROR:
		return SQL_ERROR;
	}

	sql = 
		"SELECT * "
		"FROM `user` "
		"WHERE (`id` = ? AND `password` = ?)"
		";";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

	sqlite3_bind_int(stmt, 1, uid);
	sqlite3_bind_text(stmt, 2, pwd.c_str(), pwd.length(), NULL);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		std::fprintf(stdout, "User(%d) login successfully.\n", uid);
		rtc =  SQL_SUCCESS;
		break;
	case SQLITE_DONE:
		std::fprintf(stdout, "User(%d) login failed, account or password is incorrect.\n", uid);
		rtc = SQL_FAIL;
		break;
	case SQLITE_BUSY:
	case SQLITE_LOCKED:
	default:
		std::fprintf(stderr, "SQL error: %s. 'userLogin'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}

int CSQLService::addUser(CUserData ud)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int rid = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	std::string sql;
	time_t ts;
	ts = time(NULL);
	std::fprintf(stdout, "User `%s` adding...\n", ud.name.c_str());

	
	
	sql =
		"INSERT INTO `user` "
		"(name, password, groupid, creator, register) "
		"VALUES("
		"?, lower(?), ?, ?, ?"
		");";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

	sqlite3_bind_text(stmt, 1, ud.name.c_str(), ud.name.length(), NULL);
	sqlite3_bind_text(stmt, 2, ud.pwd.c_str(), ud.pwd.length(), NULL);
	sqlite3_bind_int(stmt, 3, ud.groupid);
	sqlite3_bind_int(stmt, 4, ud.isCreator);
	sqlite3_bind_int(stmt, 5, (int)ts);
	
	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		rid= (int)sqlite3_last_insert_rowid(db);
		std::fprintf(stdout, "User `%s`(%d) add successfully.\n", ud.name.c_str(), rid);
		rtc = rid;
		break;
	default:
		switch (sqlite3_extended_errcode(db))
		{
		case SQLITE_CONSTRAINT_UNIQUE:
			std::fprintf(stderr, "User `%s` add failed. User exist.\n", ud.name.c_str());
			rtc = SQL_REGISTER_USER_EXIST;
			break;
		case SQLITE_CONSTRAINT_FOREIGNKEY:
			std::fprintf(stderr, "User `%s` add failed. Incorrect group.\n", ud.name.c_str());
			rtc = SQL_REGISTER_GROUP_NOT_EXIST;
			break;
		default:
			std::fprintf(stderr, "SQL error: %s. 'addUser'\n", sqlite3_errmsg(db));
			rtc = SQL_ERROR;
			break;
		}
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::addmeeting(CMeetingData md, int *ud)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc;
	int mid;
	sqlite3_stmt* stmt = NULL;
	char* sql;

	std::fprintf(stdout, "Meeting `%s` creating...\n", md.title);

	sql =
		"INSERT INTO `meeting`"
		"(title, content, memAmount, admin) "
		"VALUES("
		"?, ?, ?, ?"
		");";

	mutex_LOCK(db);
	sqlite3_exec(db, "begin;", 0, 0, 0);

	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	
	sqlite3_bind_text(stmt, 1, md.title.c_str(), md.title.length(), NULL);
	sqlite3_bind_text(stmt, 2, md.content.c_str(), md.content.length(), NULL);
	sqlite3_bind_int(stmt, 3, md.memAmount);
	sqlite3_bind_int(stmt, 4, md.admin);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Meeting `%s` created successfully.\n", md.title.c_str());
		std::fprintf(stdout, "Meeting `%s`'s adding member...\n", md.title.c_str());

		mid = (int)sqlite3_last_insert_rowid(db);

		switch (CSQLService::insertMember(mid, md.memAmount, ud))
		{
		case SQLITE_DONE:
			rtc = mid;
			std::fprintf(stdout, "Meeting `%s`'s added member successfully.\n", md.title.c_str());
			std::fprintf(stdout, "Meeting `%s`'s adding options of time voting...\n", md.title.c_str());
			break;
		case SQLITE_ERROR:
			rtc = SQL_ERROR;
			break;
		}
		break;
	default:
		switch (sqlite3_extended_errcode(db))
			{
			case SQLITE_CONSTRAINT_FOREIGNKEY:
				std::fprintf(stderr, "Meeting `%s` created failed. Incorrect administrator, can't find user.\n", md.title.c_str());
				rtc = SQL_MEETING_USER_NOT_EXIST;
				break;
			default:
				std::fprintf(stderr, "SQL error: %s. 'addMeeting'\n", sqlite3_errmsg(db));
				rtc = SQL_ERROR;
				break;
			}
	}

	sqlite3_finalize(stmt);
	sqlite3_exec(db, "commit;", 0, 0, 0);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::addGroup(std::string gn)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	char *sql = 
		"INSERT INTO `group` "
		"(name) "
		"VALUES(?)"
		";";

	mutex_LOCK(db);
	
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, gn.c_str(), gn.length(), NULL);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stderr, "Group `%s` add successfully.\n", gn.c_str());
		rtc = SQL_SUCCESS;
		break;
	default:
		switch (sqlite3_extended_errcode(db))
		{
		case SQLITE_CONSTRAINT_UNIQUE:
			std::fprintf(stderr, "Group `%s` add failed. Group exist.\n", gn.c_str());
			rtc = SQL_ADDGROUP_GROUP_EXIST;
			break;
		default:
			std::fprintf(stderr, "SQL error: %s. 'addGroup'\n", sqlite3_errmsg(db));
			rtc = SQL_ERROR;
			break;
		}
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}

int CSQLService::deleteGroup(int gid)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	std::string _gid = std::to_string(gid);
	std::string sql;

	sql=
		"DELETE FROM `group` "
		"WHERE("
		"id = " + _gid +
		");";

	std::fprintf(stdout, "Group `%d` deleting...\n", gid);

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stderr, "Group `%d` delete successfully.\n", gid);
		rtc = SQL_SUCCESS;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'deleteGroup'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;

	/*default:
		switch (sqlite3_extended_errcode(db))
		{
		case SQLITE_CONSTRAINT_FOREIGNKEY:
			std::fprintf(stderr, "Group `%s` delete failed. Group has foreignkey problem.\n", gid);
			break;
		default:
			std::fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
			rtc = SQL_ERROR;
			break;
		}*/
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}

int CSQLService::getGroups(CGroupData_arr *gds)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int row = 0;
	sqlite3_stmt *stmt = NULL;
	CGroupData_arr _gds;
	char* sql;
	
	sql =
		"SELECT id, name "
		"FROM `group`;";

	row = getRow(ROW_GROUP);

	switch (row)
	{
	case SQL_ERROR:
		rtc = SQL_ERROR;
		break;
	case SQL_FAIL:
		std::fprintf(stderr, "Any group is not in database\n");
		rtc = SQL_FAIL;
		break;
	default:
		*gds = std::make_unique<CGroupData[]>(row);
		_gds =  std::move(*gds);
		cout << sizeof(_gds.get());
		mutex_LOCK(db);
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

		for (int i = 0; i < row; i++)
		{
			switch (sqlite3_step(stmt))
			{
			case SQLITE_ROW:
				_gds[i].id = sqlite3_column_int(stmt, 0);
				_gds[i].name = (char*)sqlite3_column_text(stmt, 1);
				break;
			default:
				std::fprintf(stderr, "SQL error: %s. getGroups\n", sqlite3_errmsg(db));
				rtc = SQL_ERROR;
				i = row;
				break;
			}
		}
		if (_gds != NULL)
		{
			std::fprintf(stderr, "There are/is %d group(s) in database.\n", row);
			rtc = row;
		}
		mutex_UNLOCK(db);
		sqlite3_finalize(stmt);
	}
	*gds = std::move(_gds);
	return rtc;
}
int CSQLService::getUserMeeting(int userid, CMeetingData_arr *mds)
{
	int rtc = SQL_ERROR;
	int row;

	sqlite3_stmt *stmt;
	std::string uid;
	std::string sql;
	CMeetingData_arr _mds;
	/*sql =
	"SELECT * "
	"FROM `meeting` "
	"WHERE id IN "
	"(SELECT meetingid FROM `meetingmember` "
	"WHERE userid = ?)"
	";"
	;
	sql =
	"SELECT * "
	"FROM `meeting` m "
	"INNER JOIN meetingmember mb "
	"ON mb.userid = ? AND mb.meetingid = m.id;";
	*/
	uid = std::to_string(userid);
	sql =
		"SELECT * "
		"FROM `meeting`, `meetingmember`"
		"WHERE meetingmember.userid = " + uid + " "
		"AND meetingmember.meetingid = meeting.id;";

	std::fprintf(stdout, "Searing Meeting of user's id(%d)...\n", userid);

	row = getRow(sql);

	switch (row)
	{
	case SQL_ERROR:
		rtc = SQL_ERROR;
		break;
	case SQL_FAIL:
		std::fprintf(stderr, "User id(%d) don't have any meeting.\n", userid);
		rtc = SQL_FAIL;
		break;
	default:
		*mds = std::make_unique<CMeetingData[]>(row);
		_mds = std::move(*mds);

		mutex_LOCK(db);

		sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

		for (int i = 0; i < row; i++)
		{
			switch (sqlite3_step(stmt))
			{
			case SQLITE_ROW:

				_mds[i].id = sqlite3_column_int(stmt, 0);
				_mds[i].title = (char*)sqlite3_column_text(stmt, 1);
				_mds[i].content = (char*)sqlite3_column_text(stmt, 2);
				_mds[i].memAmount = sqlite3_column_int(stmt, 3);
				_mds[i].start = sqlite3_column_int(stmt, 4);
				_mds[i].end = sqlite3_column_int(stmt, 5);
				_mds[i].admin = sqlite3_column_int(stmt, 6);

				break;
			default:
				std::fprintf(stderr, "SQL error: %s. 'getUserMeeting'\n", sqlite3_errmsg(db));
				rtc = SQL_ERROR;
				i = row;
				break;
			}
		}
		if (_mds != NULL)
		{
			std::fprintf(stderr, "User id(%d) have %d search result(s).\n", userid, row);
			rtc = row;
		}

		mutex_UNLOCK(db);
		sqlite3_finalize(stmt);
	}
	*mds = std::move(_mds);
	return rtc;
}
int CSQLService::getUserData(int userid, CUserData *ud)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	char *sql;

	sql =
		"SELECT * FROM `user` "
		"WHERE id = ?;";
	
	std::fprintf(stdout, "Searching data of user's id(%d).\n", userid);

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

	sqlite3_bind_int(stmt, 1, userid);
	
	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		ud->id = sqlite3_column_int(stmt, 0);
		ud->name = (char*)sqlite3_column_text(stmt, 1);
		ud->groupid = sqlite3_column_int(stmt, 3);
		ud->isCreator = sqlite3_column_int(stmt, 4);
		std::fprintf(stdout, "User id(%d) searched successfully.\n", userid);
		rtc = SQL_SUCCESS;
		break;
	case SQLITE_DONE:
		break;
		std::fprintf(stderr, "User id(%d) isn't found.\n", userid);
		rtc = SQL_ERROR;
	default:
		std::fprintf(stderr, "SQL error: %s. 'getUserData'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}
	mutex_UNLOCK(db);
	sqlite3_finalize(stmt);
	return rtc;

}
int CSQLService::getUsersData(CUserData_arr *uds)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int row = 0;
	sqlite3_stmt *stmt = NULL;
	CUserData_arr _uds;
	std::string sql;

	sql =
		"SELECT * FROM `user`;";

	row = getRow(sql);

	switch (row)
	{
	case SQL_ERROR:
		rtc = SQL_ERROR;
		break;
	case SQL_FAIL:
		std::fprintf(stderr, "Any user is not in database.\n");
		rtc = SQL_FAIL;
		break;
	default:
		*uds = std::make_unique<CUserData[]>(row);
		_uds = std::move(*uds);

		mutex_LOCK(db);

		sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

		for (int i = 0; i < row; i++)
		{
			switch (sqlite3_step(stmt))
			{
			case SQLITE_ROW:

				_uds[i].id			= sqlite3_column_int(stmt, 0);
				_uds[i].name			= (char*)sqlite3_column_text(stmt, 1);
				_uds[i].groupid		= sqlite3_column_int(stmt, 3);
				_uds[i].isCreator	= sqlite3_column_int(stmt, 4);
				
				break;
			default:
				std::fprintf(stderr, "SQL error: %s. 'getUsersData'\n", sqlite3_errmsg(db));
				rtc = SQL_ERROR;
				i = row;
				break;
			}
		}
		if (_uds != NULL)
		{
			std::fprintf(stderr, "There are/is %d user(s) in database.\n", row);
			rtc = row;
		}

		mutex_UNLOCK(db);
		sqlite3_finalize(stmt);
	}
	*uds = std::move(_uds);
	return rtc;
}
int CSQLService::getMeetingMembData(int mid, CUserData_arr *uds)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int row;
	sqlite3_stmt *stmt;
	CUserData_arr _uds;
	std::string _mid;
	std::string sql;
	
	_mid = std::to_string(mid);

	sql =
		"SELECT * "
		"FROM `user`, `meetingmember`"
		"WHERE meetingmember.meetingid = "+ _mid + " "
		"AND meetingmember.userid = user.id;";

	std::fprintf(stdout, "Searing users of meeting's id(%d)...\n", mid);

	row = getRow(sql);

	switch (row)
	{
	case SQL_ERROR:
		rtc = SQL_ERROR;
		break;
	case SQL_FAIL:
		std::fprintf(stderr, "Meeting id(%d) don't have any member.\n", mid);
		rtc = SQL_FAIL;
		break;
	default:
		*uds = std::make_unique<CUserData[]>(row);
		_uds = std::move(*uds);

		mutex_LOCK(db);

		sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

		for (int i = 0; i < row; i++)
		{
			switch (sqlite3_step(stmt))
			{
			case SQLITE_ROW:

				_uds[i].id = sqlite3_column_int(stmt, 0);
				_uds[i].name = (char*)sqlite3_column_text(stmt, 1);
				_uds[i].groupid = sqlite3_column_int(stmt, 3);
				_uds[i].isCreator = sqlite3_column_int(stmt, 4);

				break;
			default:
				std::fprintf(stderr, "SQL error: %s. 'getMeetingMemData'\n", sqlite3_errmsg(db));
				rtc = SQL_ERROR;
				i = row;
				break;
			}
		}
		if (_uds != NULL)
		{
			std::fprintf(stderr, "Meeting id(%d) has %d search result(s).\n", mid, row);
			rtc = row;
		}
		mutex_UNLOCK(db);
		sqlite3_finalize(stmt);
	}
	*uds = std::move(_uds);
	return rtc;
}
int CSQLService::getRow(int table)
{
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt;
	std::string _table;
	std::string sql;
	switch (table)
	{
	case ROW_MEETING:
		_table = "meeting";
		break;
	case ROW_USER:
		_table = "user";
		break;
	case ROW_GROUP:
		_table = "group";
		break;
	default:
		return 0;
	}

	mutex_LOCK(db);

	sql = "SELECT COUNT(*) FROM `" + _table + "`;";

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		rtc = sqlite3_column_int(stmt, 0);
		break;
	default:
		cout << sqlite3_extended_errcode(db);
		std::fprintf(stderr, "SQL error: %s. 'getRow(int)'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}

	mutex_UNLOCK(db);
	sqlite3_finalize(stmt);
	return rtc;
}
int CSQLService::getRow(std::string countSql)
{
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	int star;

	star = countSql.find('*');
	if (star != std::string::npos)
		countSql.replace(star, 1, "COUNT(*)");

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, countSql.c_str(), strlen(countSql.c_str()), &stmt, NULL);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		rtc = sqlite3_column_int(stmt, 0);
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'getRow(string)'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}

	mutex_UNLOCK(db);
	sqlite3_finalize(stmt);
	return rtc;
}

int CSQLService::updateGroupData(int gid, std::string name)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	std::string sql;

	std::fprintf(stderr, "Updating group(%d)...\n", gid);

	sql =
		"UPDATE `group` "
		"SET `name` = ? "
		"WHERE `id` = ?;";

	switch (hasGroup(gid))
	{
	case SQL_FAIL:
		std::fprintf(stdout, "Group(%d) is not exist.\n", gid);
		return SQL_UPDATE_GROUP_NOT_EXIST;
	case SQL_ERROR:
		return SQL_ERROR;
	}

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), NULL);
	sqlite3_bind_int(stmt, 2, gid);
	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update group(%d) successfully.\n", gid);
		rtc = SQL_SUCCESS;
		break;
	default:
		switch (sqlite3_extended_errcode(db))
		{
		case SQLITE_CONSTRAINT_UNIQUE:
			std::fprintf(stderr, "Update group(%d) failed. Exist same name of group.\n", gid);
			rtc = SQL_UPDATE_GROUP_EXIST;
			break;
		default:
			std::fprintf(stderr, "SQL error: %s. 'updateGroup'\n", sqlite3_errmsg(db));
			rtc = SQL_ERROR;
			break;
		}
		break;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::updateUserName(int uid, std::string name)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	std::string sql;

	std::fprintf(stderr, "Updating user(%d)'s name...\n", uid);

	switch (hasUser(uid))
	{
	case SQL_FAIL:
		std::fprintf(stdout, "User(%d) is not exist.\n", uid);
		return SQL_UPDATE_GROUP_NOT_EXIST;
	case SQL_ERROR:
		return SQL_ERROR;
	}
	sql =
		"UPDATE `user` "
		"SET `name` = ? "
		"WHERE `id` = ?;";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), NULL);
	sqlite3_bind_int(stmt, 2, uid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update user(%d)'s name successfully.\n", uid);
		rtc = SQL_SUCCESS;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'updateUserName'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::updateUserGroup(int uid, int group)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int o = 0;
	sqlite3_stmt *stmt = NULL;
	std::string sql;

	std::fprintf(stderr, "Updating user(%d)'s group...\n", uid);

	switch (hasUser(uid))
	{
	case SQL_FAIL:
		std::fprintf(stdout, "User(%d) is not exist.\n", uid);
		return SQL_UPDATE_GROUP_NOT_EXIST;
	case SQL_ERROR:
		return SQL_ERROR;
	}
	sql =
		"UPDATE `user` "
		"SET `groupid` = ? "
		"WHERE `id` = ?;";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, group);
	sqlite3_bind_int(stmt, 2, uid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update user(%d)'s group successfully.\n", uid);
		rtc = SQL_SUCCESS;
		break;
	default:
		switch (sqlite3_extended_errcode(db))
		{
		case SQLITE_CONSTRAINT_FOREIGNKEY:
			std::fprintf(stderr, "Update user(%d) failed. Incorrect group.\n", uid);
			rtc = SQL_UPDATE_GROUP_NOT_EXIST;
			break;
		default:
			std::fprintf(stderr, "SQL error: %s. 'updateUserGroup'\n", sqlite3_errmsg(db));
			rtc = SQL_ERROR;
			break;
		}
		break;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::updateUserCreator(int uid, int isCreator)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int o = 0;
	sqlite3_stmt *stmt = NULL;
	std::string sql;

	std::fprintf(stderr, "Updating user(%d)'s creator permission...\n", uid);

	if (isCreator > 1 || isCreator < 0)
	{
		std::fprintf(stderr, "Update user(%d) failed. Incorrect creator.\n", uid);
		return SQL_UPDATE_INCORRECT_CREATOR;
	}

	switch (hasUser(uid))
	{
	case SQL_FAIL:
		std::fprintf(stdout, "User(%d) is not exist.\n", uid);
		return SQL_UPDATE_GROUP_NOT_EXIST;
	case SQL_ERROR:
		return SQL_ERROR;
	}
	sql =
		"UPDATE `user` "
		"SET `creator` = ? "
		"WHERE `id` = ?;";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, isCreator);
	sqlite3_bind_int(stmt, 2, uid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update user(%d)'s creator permission successfully.\n", uid);
		rtc = SQL_SUCCESS;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'updateUserCreator'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::updateUserPwd(int uid, std::string pwd)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int o = 0;
	sqlite3_stmt *stmt = NULL;
	std::string sql;

	std::fprintf(stderr, "Updating user(%d)'s password...\n", uid);

	switch (hasUser(uid))
	{
	case SQL_FAIL:
		std::fprintf(stdout, "User(%d) is not exist.\n", uid);
		return SQL_UPDATE_GROUP_NOT_EXIST;
	case SQL_ERROR:
		return SQL_ERROR;
	}
	sql =
		"UPDATE `user` "
		"SET `password` = ?"
		"WHERE `id` = ? ;";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, pwd.c_str(), pwd.length(), NULL);
	sqlite3_bind_int(stmt, 2, uid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update user(%d)'s password successfully.\n", uid);
		rtc = SQL_SUCCESS;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'updateUserPwd'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::updateMeetingTime(int mid, int _time, TimeType tt)
{
	std::lock_guard<std::mutex> lck_guard(sync_mtx);
	int rtc = SQL_ERROR;
	int o = 0;
	sqlite3_stmt *stmt = NULL;
	std::string sql;
	std::string _tt;
	time_t ts;
	ts = time(NULL);
	switch (tt)
	{
	case START:
		_tt = "start";
		break;
	case END:
		_tt = "end";
		break;
	}

	std::fprintf(stderr, "Updating meeting(%d)'s %s...\n", mid, _tt.c_str());

	sql =
		"UPDATE `meeting` "
		"SET `" + _tt + "` = ?"
		"WHERE `id` = ?;";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
	if (tt == TimeType::END)
		sqlite3_bind_int(stmt, 1, (int)ts);
	else
		sqlite3_bind_int(stmt, 1, _time);
	sqlite3_bind_int(stmt, 2, mid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update meeting(%d)'s %s successfully.\n", mid, _tt.c_str());
		rtc = SQL_SUCCESS;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'updateMeetingTime'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::setVoted(int mid, int uid)
{
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt;
	std::string sql;

	sql =
		"UPDATE `meetingmember`"
		"SET `isVoted` = 1 "
		"WHERE `userid` = ? AND `meetingid` =?;";

	sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, uid);
	sqlite3_bind_int(stmt, 2, mid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_DONE:
		std::fprintf(stdout, "Update meeting(%d)'s setVoted of user(%d) successfully.\n", mid, uid);
		rtc = SQL_SUCCESS;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'setVoted'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
		break;
	}
	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}
int CSQLService::isVoted(int mid, int uid)
{
	std::string sql;
	int rtc = SQL_ERROR;
	sqlite3_stmt* stmt = NULL;

	sql =
		"SELECT * "
		"FROM `meetingmember` "
		"WHERE (`meetingid` = ? AND `userid` = ? AND `isVoted` = 1)"
		";";

	mutex_LOCK(db);

	sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);

	sqlite3_bind_int(stmt, 1, mid);
	sqlite3_bind_int(stmt, 2, uid);

	switch (sqlite3_step(stmt))
	{
	case SQLITE_ROW:
		rtc = SQL_SUCCESS;
		break;
	case SQLITE_DONE:
		rtc = SQL_FAIL;
		break;
	default:
		std::fprintf(stderr, "SQL error: %s. 'hasUser'\n", sqlite3_errmsg(db));
		rtc = SQL_ERROR;
	}

	sqlite3_finalize(stmt);
	mutex_UNLOCK(db);
	return rtc;
}

int CSQLService::insertMember(int mid, int memAmount, int *ud)
{
	int rtc = SQL_ERROR;
	sqlite3_stmt *stmt = NULL;
	char* sql;

	sql =
		"INSERT INTO `meetingmember`"
		"(meetingid, userid)"
		"VALUES(?, ?)"
		";";

	mutex_LOCK(db);
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

	for (int i = 0; i < memAmount; i++)
	{
		sqlite3_reset(stmt);
		sqlite3_bind_int(stmt, 1, mid);
		sqlite3_bind_int(stmt, 2, ud[i]);

		if (SQLITE_DONE != sqlite3_step(stmt))
		{
			std::fprintf(stderr, "SQL error: %s. 'insertMember'\n", sqlite3_errmsg(db));
			rtc = SQL_ERROR;
			break;
		}
	}

	if (SQLITE_DONE == sqlite3_errcode(db))
		rtc = SQLITE_DONE;

	sqlite3_finalize(stmt); 
	mutex_UNLOCK(db);
	return rtc;
}