#include"CVideoRecorder.h"
int w = 640;
int h = 480;
int ret;
PicItem item;
bool saveFinished;
bool isRecording;
void record264File(int w, int h, char* filename)
{
	CH264Encoder encoder;
	CImageFormatConverter cvt;
	AVFrame *out;
	uint8_t *out_h264raw;
	ofstream file(filename, ios::binary);
	encoder.setupEncoder(400000, w, h, 30, 5, 3, 0, 30, 50, X264_ENCODE_ULTRAFAST);
	CCameraCapturer cap;
	cap.start();
	cvt.setupConverter(w, h, w, h, AV_PIX_FMT_BGR24, AV_PIX_FMT_YUV420P);

	while (isRecording)
	{
		item = cap.getItem();
		cvt.convert((uint8_t*)item.pixels, &out);  //convert rgb24->yuv420 

		ret = encoder.encodeFragment(out->data[0], &out_h264raw);
		if (ret > 0)
			file.write((char*)out_h264raw, ret);
	}
	do
	{
		ret = encoder.flushFrameOut(&out_h264raw);
		if (ret > 0)
			file.write((char*)out_h264raw, ret);
	} while (ret);
	file.flush();
	file.close();
	cout << "CVideoRecorder:Video Recording has Finished!\n";
	saveFinished = true;
}
CVideoRecorder::CVideoRecorder()
{
	isRecording=false;
	saveFinished=false;
}
void CVideoRecorder::start(int w, int h, char* filename)
{
	recordThread = new thread(record264File, w, h, filename);
	isRecording = true;
	saveFinished = false;
	printf("CVideoRecorder:Record Started\n");
}
void CVideoRecorder::stop()
{
	isRecording = false;
	while (!saveFinished){ Sleep(100); };
	free(recordThread);
	printf("CVideoRecorder:Record Stoped\n");
}
