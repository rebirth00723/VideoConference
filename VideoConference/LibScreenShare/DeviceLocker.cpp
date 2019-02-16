#include "DeviceLocker.h"
static bool KeyBoardLock;
static HHOOK LLKeyBoardHook;
static bool MouseLock;
static HHOOK LLMouseHook;

int CKeyBoardLocker::count;
int CMouseLocker::count;
void CDeviceLocker::messageLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
CKeyBoardLocker::CKeyBoardLocker()
{

}
CKeyBoardLocker::~CKeyBoardLocker()
{
	release();
}
void CKeyBoardLocker::release()
{
	if (LLKeyBoardHook)
	{
		UnhookWindowsHookEx(LLKeyBoardHook);
		LLKeyBoardHook = 0;
		count--;
	}
	KeyBoardLock = false;
}
void CKeyBoardLocker::lock()
{
	if (count == 0)
	{
		LLKeyBoardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
		count++;
		KeyBoardLock = true;
	}
	else
	{
		printf("CKeyBoardLocker:You can have only one keyboard locker.");
		KeyBoardLock = false;
	}
}
void CKeyBoardLocker::unlock()
{
	release();
}
CMouseLocker::CMouseLocker()
{

}
CMouseLocker::~CMouseLocker()
{
	release();
}
void CMouseLocker::release()
{
	if (LLMouseHook)
	{
		UnhookWindowsHookEx(LLMouseHook);
		LLMouseHook = 0;
		count--;
	}
}
void CMouseLocker::lock()
{
	if (count == 0)
	{
		LLMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
		MouseLock = true;
		count++;
	}
	else
	{
		printf("CKeyBoardLocker:You can have only one keyboard locker.");
		MouseLock = false;
	}
	
}
void CMouseLocker::unlock()
{
	release();
}


LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	return KeyBoardLock ? -1 : CallNextHookEx(LLKeyBoardHook, code, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam)
{
	return MouseLock ? -1 : CallNextHookEx(LLMouseHook, code, wParam, lParam);
}