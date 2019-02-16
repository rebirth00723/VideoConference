#ifndef CCPacketQueue_H_INCLUDED
#define CCPacketQueue_H_INCLUDED
#include <thread>
#include <mutex>
#include <condition_variable>
template<typename T>
class CPacketList
{
public:
    CPacketList(const T &dv,CPacketList *nv=NULL):
        data(dv),next(nv)
    {
    }
    T data;
    CPacketList<T> *next;
};
template<typename T>
class CPacketQueue
{
friend class CPacketList<T>;
public:
    CPacketQueue(int maxQueue=10)
    {
        root = tail = NULL;
        nb_packet = 0;
        break_flag = false;
        max_que_size = maxQueue;
    }
	void setMaxSize(int nv)
	{
		max_que_size = nv;
	}
    void push(T data)
    {
        CPacketList<T> *item = new CPacketList<T>(data);
        std::unique_lock<std::mutex> lck(sync_mutex);
		if (nb_packet >= max_que_size)
		{
			full_event.wait(lck);
		}
		if(!tail)
        {
            root = item;
        }
        else
            tail->next = item;
        tail = item;
        nb_packet++;
        empty_event.notify_one();
    }
	bool pushNonBlocking(T data)
	{
		CPacketList<T> *item = new CPacketList<T>(data);
		std::unique_lock<std::mutex> lck(sync_mutex);
		if (nb_packet >= max_que_size)
		{
			delete item;
			return false;
		}
		if (!tail)
		{
			root = item;
		}
		else
			tail->next = item;
		tail = item;
		nb_packet++;
		empty_event.notify_one();
		return true;
	}
	CPacketList<T>* getItemNonBlocking()
	{
		CPacketList<T> *out;
		std::unique_lock<std::mutex> lck(sync_mutex);
		out = root;
		if (out)
		{
			root = root->next;
			if (!root)
				tail = NULL;
			nb_packet--;
			full_event.notify_one();
		}
		return out;
	}
    CPacketList<T>* getItem()
    {
        CPacketList<T> *out;
        std::unique_lock<std::mutex> lck(sync_mutex);
        //until get item
        while(1)
        {
            out = root;
            if(out)
            {
                root = root->next;
                if(!root)
                    tail = NULL;
                nb_packet--;
                full_event.notify_one();
                break;
            }
            else
            {
                if(break_flag)
                {
                    out = NULL;
                    break;
                }
                empty_event.wait(lck);
            }
        }
        return out;
    }
	CPacketList<T>* getItemUntil(int equal_size)
	{
		CPacketList<T> *out;
		while (nb_packet < equal_size)
		{
			Sleep(10);
		}
		std::unique_lock<std::mutex> lck(sync_mutex);
		//until get item
		while (1)
		{
			out = root;
			if (out)
			{
				root = root->next;
				if (!root)
					tail = NULL;
				nb_packet--;
				full_event.notify_one();
				break;
			}
			else
			{
				if (break_flag)
				{
					out = NULL;
					break;
				}
				empty_event.wait(lck);
			}
		}
		return out;
 	}
	CPacketList<T>* flushOutItem()
	{
		CPacketList<T> *out;
		std::unique_lock<std::mutex> lck(sync_mutex);
		out = root;
		if (out)
		{
			root = root->next;
			if (!root)
				tail = NULL;
			nb_packet--;
		}
		return out;
	}
    int getSize()
    {
        std::unique_lock<std::mutex> lck(sync_mutex);
        return nb_packet;
    }
    void break_getItem()
    {
         std::unique_lock<std::mutex> lck(sync_mutex);
		 empty_event.notify_all();
		 full_event.notify_all();
		 break_flag = true;
    }
	void reset()
	{
		break_flag = false;
	}
private:
    CPacketList<T> *root,*tail;
    int nb_packet;
    int max_que_size;
    bool break_flag;
    std::mutex sync_mutex;
    std::condition_variable full_event;
    std::condition_variable empty_event;
};


#endif // CCPacketQueue_H_INCLUDED

