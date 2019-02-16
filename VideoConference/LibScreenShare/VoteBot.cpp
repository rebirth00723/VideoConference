#include "VoteBot.h"

VoteBot::VoteBot()
{

}
void VoteBot::getVoteTimes(int mid, int_arr *vts, int *count, int *deadLine)
{
	mit = Meeting.find(mid);
	*deadLine = 0;
	*count = 0;
	if (mit == Meeting.end())
	{
		return;
	}
		
	*count = Meeting[mid].size() - 2;
	
	*vts = std::make_unique<int[]>(*count);

	int i = 0;
	int *_vts = vts->get();
	for (auto vit = Meeting[mid].begin(); vit != Meeting[mid].end(); vit++)
	{
		if (vit->second == DEADLINE)
			*deadLine = vit->first;
		if (vit->second != DEADLINE && vit->second != MEMAMOUNT)
		{
			_vts[i] = vit->first;
			++i;
		}
	}
}
void VoteBot::addNewMeetingVote(CMeetingData md, int* voteTime, int count)
{
	mit = Meeting.find(md.id);
	std::thread *_thread;
	if (mit != Meeting.end())
		return;
	else
	{

		std::lock_guard<std::mutex> mLock(threadMutex);			//Lock current thread
		Votetimes.clear();
		for (int i = 0; i < count; i++)
			Votetimes[voteTime[i]] = 0;

		Meeting[md.id] = Votetimes;
		Meeting[md.id][md.deadLine] = DEADLINE;
		Meeting[md.id][md.memAmount] = MEMAMOUNT;

		threads.push_back(std::thread(&VoteBot::threadCheck, this, md.id));
		
		threads.back().detach();
	}

}
void VoteBot::voteIn(int mid, int time)
{
	std::lock_guard<std::mutex> mLock(threadMutex);			//Lock current thread
	++Meeting[mid][time];
}
void VoteBot::threadCheck(int mid)
{
	int totalVote;		//總頭票數
	int maxVote;		//最大票數
	int dealline;
	int	starttime = 0;
	int memAmount;
	bool stopFlag = true;
	std::time_t ctime;

	std::map<int, int>::const_iterator vit;

	while (stopFlag)
	{
		_sleep(1000);
		std::lock_guard<std::mutex> mLock(threadMutex);			//Lock current thread
		totalVote = 0;
		dealline = 0;
		ctime = std::time(nullptr);
		std::asctime(std::localtime(&ctime));		//get current time
		for (vit = Meeting[mid].begin(); vit != Meeting[mid].end(); vit++)
		{
			totalVote += vit->second;

			if (vit->second == DEADLINE)
				dealline = vit->first;
			else if (vit->second == MEMAMOUNT)
				memAmount = vit->first;

		}
		totalVote -= 2001;

		std::cout << "mid: " << mid << " deadline: " << dealline << " Current time: " << ctime << " memAmount " << memAmount << " totalVote: " << totalVote << std::endl;
		if (ctime > dealline)
		{
			//do sql function
			std::cout << "time out!" << std::endl;
			maxVote = 0;
			for (vit = Meeting[mid].begin(); vit != Meeting[mid].end(); vit++)
			{
				if (vit->second != DEADLINE && vit->second != MEMAMOUNT)
				if (vit->second > maxVote)
				{
					maxVote = vit->second;
					starttime = vit->first;
					
				}
			}
			Meeting.erase(mid);
			stopFlag = false;
			sql.updateMeetingTime(mid, starttime, TimeType::START);
			break;
		}

		if (totalVote >= memAmount)
		{
			//do sql function
			std::cout << "All members has already voted!" << std::endl;
			maxVote = 0;
			for (vit = Meeting[mid].begin(); vit != Meeting[mid].end(); vit++)
			{
				if (vit->second != DEADLINE && vit->second != MEMAMOUNT)
				if (vit->second > maxVote)
				{
					maxVote = vit->second;
					starttime = vit->first;
				}
			}
			stopFlag = false;
			sql.updateMeetingTime(mid, starttime, TimeType::START);
			break;
		}
		std::cout << "threading.." << std::endl;
	}
}
