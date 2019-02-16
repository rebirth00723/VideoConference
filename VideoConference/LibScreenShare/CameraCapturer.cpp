#include "CameraCapturer.h"
CCameraCapturer::CCameraCapturer(int camId, int max_que_size)
{
	pic_que.setMaxSize(max_que_size);
	cam.open(camId);
	internal_Init();
	updateCapturerData();
}
CCameraCapturer::~CCameraCapturer()
{
	finish();
}
void CCameraCapturer::updateCapturerData()
{
	Mat mat;
	IplImage pic;
	if (cam.isOpened())
	{
		cam >> mat;
		pic = IplImage(mat);
		cap_height = pic.height;
		cap_width = pic.width;
		pixels_length = cap_height * cap_width * 3;
	}
	else
	{
		printf("CCameraCapturer: Your camera not working!\n");
	}
}
void CCameraCapturer::start()
{
	if (!capture_thread)
	{
		capture_thread = new std::thread(&CCameraCapturer::capture_handle, this);
	}
	else
	{
		printf("CScreenCapturer: You can't start camera capturer twice!\n");
	}
}
void CCameraCapturer::pause()
{
	stopped = true;
}
void CCameraCapturer::restart()
{
	stopped = false;
}
void CCameraCapturer::finish()
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
PicItem CCameraCapturer::getItem()
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
int CCameraCapturer::getItemCount()
{
	return pic_que.getSize();
}
int CCameraCapturer::getHeight() const
{
	return cap_height;
}
int CCameraCapturer::getWidth() const
{
	return cap_width;
}
SIZE CCameraCapturer::getSize() const
{
	return SIZE{ cap_width, cap_height };
}
void CCameraCapturer::capture_handle()
{
	finished = false;
	Mat mat;
	IplImage iplImage;
	while (1)
	{
		if (stopped || !cam.isOpened())
		{
			Sleep(30);
			continue;
		}
		else if (finished)
			break;
		cam >> mat;
		iplImage = IplImage(mat);
		picitem.pixels = (uint8_t*)malloc(pixels_length);
		memcpy(picitem.pixels, iplImage.imageData, pixels_length);
		pic_que.push(picitem);
	}
}
void CCameraCapturer::internal_Init()
{
	memset(&picitem, 0, sizeof(PicItem));
	item = NULL;
	capture_thread = NULL;
	finished = stopped = false;
	cap_height = cap_width = 0;

}