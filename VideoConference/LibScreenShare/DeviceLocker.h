#ifndef DEVICE_LOCKER_H
#define DEVICE_LOCKER_H
#include <Windows.h>
#include <stdio.h>
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam);
class CDeviceLocker
{
public:
	virtual void release() = 0;
	virtual void lock()    = 0;
	virtual void unlock()  = 0;
	static void messageLoop();
};
class CKeyBoardLocker : public CDeviceLocker
{
public:
	CKeyBoardLocker();
	~CKeyBoardLocker();
	void release();
	void lock();
	void unlock();
private:
	static int count;
};
class CMouseLocker : public CDeviceLocker
{
public:
	CMouseLocker();
	~CMouseLocker();
	void release();
	void lock();
	void unlock();
private:
	static int count;
};
#endif