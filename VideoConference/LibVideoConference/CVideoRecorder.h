#ifndef CVIDEORECORDER_H
#define CVIDEORECORDER_H
#include <stdlib.h>
#include <iostream>
#include"../LIbScreenShare/H264Codec.h"
#include"../LibScreenShare/ImageUtils.h"
#include"../LibScreenShare/CameraCapturer.h"
#include<thread>
#include<Windows.h>
using namespace std;
class CVideoRecorder
{
public:
	CVideoRecorder();
	void start(int w, int h, char* filename);
	void stop();
	thread* recordThread;
};
#endif