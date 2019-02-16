#ifndef FFMPEG_BASE_H
#define FFMPEG_BASE_H
extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
};
#include <iostream>
#include <fstream>
#include <memory.h>

class CFFmpegCodecBase
{
public:
	CFFmpegCodecBase();
	virtual void		 showProperties() const = 0;
	/*
		***Do not call this before decoder didn't decode any frame.***
		***For general use, you should decode first frame so you could get information***

		return value
		true  --> get a YUV length successful.
		false --> didn't decode anything, so the decoder couldn't give you the information.
	*/
	bool				 getYUV420PLength(size_t *YLen, size_t *UVLen) const; 
	/*
		***Do not call this before decoder didn't decode any frame.***
		***For general use, you should decode first frame so you could get information***

		return value
		true  --> get a YUV length successful.
		false --> didn't decode anything, so the decoder couldn't give you the information.
	*/
	bool				 getSize(int &w, int &h) const;
	/*
		***Do not call this before decoder didn't decode any frame.***
		***For general use, you should decode first frame so you could get information***

		return value
		true  --> get a YUV length successful.
		false --> didn't decode anything, so the decoder couldn't give you the information.
	*/
	int					 getFPS() const;
protected:
	bool virtual		 internal_Init() = 0;
	AVCodecContext		*pCodecCtx;
	AVCodec				*pCodec;
private:
	static bool			 isInitializedFFmpeg;
};
#endif