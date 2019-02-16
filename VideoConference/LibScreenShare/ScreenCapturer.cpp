#include "ScreenCapturer.h"
CScreenCapturer::CScreenCapturer(int max_que_size)
{
	pic_que.setMaxSize(max_que_size);
	internal_Init();
	updateCapturerData();
}
CScreenCapturer::~CScreenCapturer()
{
	finish();
	free(pixels);
	ReleaseDC(0, hdc);
	DeleteObject(hdc);
	DeleteObject(hMemDC);
	DeleteObject(hBitmap);
}
void CScreenCapturer::updateCapturerData()
{
	screen_width = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);
	bmp_paddings = screen_width % 4;
	pixels_length = (screen_width * 3 + bmp_paddings) * screen_height;

	BitmapFileHeader.bfType = 19778;
	BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BitmapFileHeader.bfSize = pixels_length + BitmapFileHeader.bfOffBits;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biHeight = screen_height;
	BitmapInfo.bmiHeader.biWidth = screen_width;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 24;
	BitmapInfo.bmiHeader.biSizeImage = pixels_length;
	if (pixels)
	{
		free(pixels);
		ReleaseDC(0, hdc);
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
	}
	pixels = new BYTE[pixels_length];
	lpPixelValue = NULL;
	hdc = GetDC(0);
	hMemDC = CreateCompatibleDC(hdc);
	hBitmap = CreateDIBSection(hMemDC, (BITMAPINFO*)&BitmapInfo,
		DIB_RGB_COLORS, (void**)&lpPixelValue, NULL, 0);
	SelectObject(hMemDC, hBitmap);
	picitem.width = screen_width;
	picitem.height = screen_height;
	picitem.pixels_length = pixels_length;
}
void CScreenCapturer::start()
{
	if (!capture_thread)
	{
		capture_thread = new std::thread(&CScreenCapturer::capture_handle, this);
	}
	else
	{
		printf("CScreenCapturer: You can't start screen capturer twice!\n");
	}
}
void CScreenCapturer::pause()
{
	stopped = true;
}
void CScreenCapturer::restart()
{
	stopped = false;
}
void CScreenCapturer::finish()
{
	finished = true;
	if (capture_thread)
	{
		pic_que.break_getItem();
		capture_thread->join();
		capture_thread = NULL;
	}
	while (item = pic_que.flushOutItem())
	{
		free(item->data.pixels);
		delete item;
	}
	pic_que.reset();
	item = nullptr;
}
PicItem CScreenCapturer::getItem()
{
	PicItem out = { 0 };
	if (item)
		delete item;

	if (!finished)
	{
		item = pic_que.getItem();
		if (item)
			out = item->data;
	}
	else
	{
		item = pic_que.flushOutItem();
		if (item)
			out = item->data;
	}	
	return out;
}
int CScreenCapturer::getItemCount()
{
	return pic_que.getSize();
}
int CScreenCapturer::getWidth() const
{
	return screen_width;
}
int CScreenCapturer::getHeight() const
{
	return screen_height;
}
SIZE CScreenCapturer::getSize() const
{
	return SIZE{ screen_width, screen_height };
}
void CScreenCapturer::capture_handle()
{
	//LoadCursor()
	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
	ICONINFO info;
	POINT pos;
	GetIconInfo(hCursor, &info);
	finished = false;
	while (1)
	{
		if (stopped)
		{
			Sleep(30);
			continue;
		}
		else if (finished)
			break;
		BitBlt(hMemDC, 0, 0, screen_width, screen_height, hdc,
			0, 0, SRCCOPY);
		GetCursorPos(&pos);
		DrawIcon(hMemDC, pos.x, pos.y, hCursor);
		picitem.pixels = (uint8_t*)malloc(pixels_length);
		memcpy(picitem.pixels, lpPixelValue, pixels_length);
		pic_que.push(picitem);
	}
}
void CScreenCapturer::internal_Init()
{
	pixels = NULL;
	lpPixelValue = NULL;
	capture_thread = NULL;
	memset(&BitmapFileHeader, 0, sizeof(BitmapFileHeader));
	memset(&BitmapInfo, 0, sizeof(BitmapInfo));
	memset(&picitem, 0, sizeof(PicItem));
	hdc = NULL;
	hBitmap = NULL;
	hMemDC = NULL;
	item = NULL;
	stopped = finished = false;
	screen_height = screen_width = bmp_paddings = pixels_length = 0;
}
