#ifndef ABSTRACTCAPTURER_H
#define ABSTRACTCAPTURER_H
//Note this file is only for windows
//If you can implement linux version, please help this file.
#ifdef _MSC_VER
#include <thread>
#include <Windows.h>
#elif
//help linux version header here
#endif
#include "CPacketQueue.h"

typedef struct
{
	int width;
	int height;
	int pixels_length;
	uint8_t *pixels;
}PicItem;

class CAbstractCapturer
{
public:
	/*
	---Important---
	This function is used for getting new resolution of screen(or camera),
	and 
	screen : updating the data of Bitmap header used for screen-capturing.
	camera : updating the data of Image Size used for camera-capturing.
	*/
	virtual void						updateCapturerData() = 0;
	/*
	---Important---
	Starting the capture_handle() thread,
	Note that this function is only invoked once.
	*/
	virtual void						start() = 0;
	/*
	---Common use---
	Pause the capture_handle() thread,
	Using restart() for restarting.
	*/
	virtual void						pause() = 0;
	/*
	---Common use---
	Restart the capture_handle() thread
	that was stopped by you.
	*/
	virtual void						restart() = 0;
	/*
	---Common use---
	Use this function to terminate capture_handle() thread.
	Once you terminate the screen(or camera) capturer,
	that is, it's no longer to be used.
	*/
	virtual void						finish() = 0;
	/*
	---Common use---
	Use this function to get the item that was put into the queue.
	*/
	virtual PicItem						getItem() = 0;
	virtual int							getItemCount() = 0;
	virtual int							getHeight() const = 0;
	virtual int							getWidth() const = 0;
	virtual SIZE						getSize() const = 0;
};

#endif