#pragma once

#include <iostream>
#include <map>
#include <thread>
#include <ctime>
#include <mutex>
#include <vector>
#include <../LibVideoConference/SQLService.h>
#define DEADLINE	1000
#define MEMAMOUNT	1001

class VoteBot
{

public:
	VoteBot();
	std::mutex threadMutex;

	std::map<int, std::map<int, int>>Meeting;
	std::map<int, int>Votetimes;
	std::map<int, std::map<int, int>>::const_iterator mit;

	void getVoteTimes(int mid, int_arr *vts, int *count, int *deadLine);
	void addNewMeetingVote(CMeetingData, int*, int);
	void voteIn(int, int);
	void threadCheck(int mid);

	std::vector<std::thread>threads;
private:
	CSQLService		sql;

};
