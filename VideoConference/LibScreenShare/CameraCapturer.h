#ifndef CAMERA_CAPTURER_H
#define CAMERA_CAPTURER_H
#include "AbstractCapturer.h"
#include "opencv2\highgui\highgui.hpp"
#include"opencv\cv.h"
using namespace cv;

class CCameraCapturer :
	public CAbstractCapturer
{
public:
	CCameraCapturer(int camId = 0, int max_que_size = 30);
	~CCameraCapturer();
	void						updateCapturerData();
	void						start();
	void						pause();
	void						restart();
	void						finish();
	PicItem						getItem();
	int							getItemCount();
	int							getWidth() const;
	int							getHeight() const;
	SIZE						getSize() const;
	
private:
	void						internal_Init();
	void						capture_handle();
	int							cap_width;
	int							cap_height;
	int							pixels_length;
	bool						finished;
	bool						stopped;
	std::thread				   *capture_thread;
	VideoCapture				cam;
	PicItem						picitem;
	CPacketQueue<PicItem>		pic_que;
	CPacketList<PicItem>	   *item;
};

#endif 