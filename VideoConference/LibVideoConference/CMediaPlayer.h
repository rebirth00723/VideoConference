#ifndef CMEDIAPLAYER_H
#define CMEDIAPLAYER_H
#include <QtWidgets/QApplication>
#include"qwidget.h"
#include"iostream"
#include"../LibScreenShare/H264Codec.h"
#include"../LibScreenShare/MPlayer.h"
#include"../LibScreenShare/ImageUtils.h"
#include"CVideoSurface.h"
#include"qlayout.h"
//#include<thread>
#include"../LibScreenShare/MediaService.h"

#include"qtimer.h"
#include"qfile.h"
#include"qbuffer.h"
using namespace std;
class CStreamPlayer:public QWidget
{
	Q_OBJECT
public:
	CStreamPlayer();
	void setAddr(char* ip, char* aip = "disable");
	void play264File(char*filename,int w,int h);
	bool YV12ToBGR24_FFmpeg(unsigned char* pYUV, unsigned char* pBGR24, int width, int height);

	CVideoSurface* nvs;
	QGridLayout *layout;
	char* tempip;
	char* tempaip;

	std::thread *Video_thread;
	std::thread *Audio_thread;

	bool meetend;
	bool closed;
	~CStreamPlayer();

	int memberID;

};
#endif