#ifndef SCREEN_CAPTURER_H
#define SCREEN_CAPTURER_H
#include "AbstractCapturer.h"

class CScreenCapturer:
	public CAbstractCapturer
{
public:
	CScreenCapturer(int max_que_size = 30);
	~CScreenCapturer();
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
	int							screen_width;
	int							screen_height;
	int							pixels_length;
	int							bmp_paddings;
	bool						finished;
	bool						stopped;
	PicItem						picitem;
	std::thread				   *capture_thread;
	CPacketQueue<PicItem>		pic_que;
	CPacketList<PicItem>	   *item;
	BYTE					   *pixels;
	BYTE					   *lpPixelValue;
	HDC							hdc;
	HDC							hMemDC;
	HBITMAP						hBitmap;
	BITMAPINFO					BitmapInfo;
	BITMAPFILEHEADER			BitmapFileHeader;
};
#endif