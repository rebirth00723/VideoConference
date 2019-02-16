#include"CMediaPlayer.h"
static int idcnt=0;
QBuffer buf;
bool CStreamPlayer::YV12ToBGR24_FFmpeg(unsigned char* pYUV, unsigned char* pBGR24, int width, int height)
{
	if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
		return false;
	//int srcNumBytes,dstNumBytes;
	//uint8_t *pSrc,*pDst;
	AVPicture pFrameYUV, pFrameBGR;

	//pFrameYUV = avpicture_alloc();
	//srcNumBytes = avpicture_get_size(PIX_FMT_YUV420P,width,height);
	//pSrc = (uint8_t *)malloc(sizeof(uint8_t) * srcNumBytes);
	avpicture_fill(&pFrameYUV, pYUV, AV_PIX_FMT_YUV420P, width, height);

	//U,V¤¬?
	uint8_t * ptmp = pFrameYUV.data[1];
	pFrameYUV.data[1] = pFrameYUV.data[2];
	pFrameYUV.data[2] = ptmp;

	//pFrameBGR = avcodec_alloc_frame();
	//dstNumBytes = avpicture_get_size(PIX_FMT_BGR24,width,height);
	//pDst = (uint8_t *)malloc(sizeof(uint8_t) * dstNumBytes);
	avpicture_fill(&pFrameBGR, pBGR24, AV_PIX_FMT_BGR24, width, height);

	struct SwsContext* imgCtx = NULL;
	imgCtx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGR24, SWS_BILINEAR, 0, 0, 0);

	if (imgCtx != NULL){
		sws_scale(imgCtx, pFrameYUV.data, pFrameYUV.linesize, 0, height, pFrameBGR.data, pFrameBGR.linesize);
		if (imgCtx){
			sws_freeContext(imgCtx);
			imgCtx = NULL;
		}
		return true;
	}
	else{
		sws_freeContext(imgCtx);
		imgCtx = NULL;
		return false;
	}
}
void CStreamPlayer::play264File(char*filename,int w,int h)
{
	CH264Decoder decoder;
	decoder.setupDecoder();
	decoder.decodeFile(filename, "temp.yuv");
	CVideoSurface nvs(w, h);
	layout->addWidget(&nvs);
	nvs.show();
	this->show();

	FILE* yuvFile;
	yuvFile = fopen("temp.yuv", "rb");

	long length = 0;
	fseek(yuvFile, 0, SEEK_END);
	length = ftell(yuvFile);
	rewind(yuvFile);
	int i = length / w*h*3;

	while (i--)//!feof(yuvFile)
	{
		char* buf = (char*)malloc(sizeof(char*) * w*h*3);
		char* rgb = (char*)malloc(sizeof(char*) * w*h*3);
		fread(buf, 1, w*h*3, yuvFile);
		YV12ToBGR24_FFmpeg((unsigned char*)buf, (unsigned char*)rgb, w, h);
		QImage *im = new QImage((uchar*)rgb, w, h, QImage::Format_RGB888);
		nvs.updateImageRefresh(*im);
		qApp->processEvents();
		Sleep(30);
		free(buf);
		free(rgb);
	}
}

void audio_handle(void *param)
{
	CStreamPlayer* mthis = (CStreamPlayer*)param;
	
	char* ip = mthis->tempaip;
	int id = idcnt;

	char c = char(id + '0');
	char* sdpfilename = (char*)malloc(1000);
	sprintf(sdpfilename, "%ca.sdp", c);

	CMediaSender AudioSender;
	AudioSender.setupProperties(DISABLE, ip);
	AudioSender.writeAudioSDPFile(sdpfilename);

	int ret, w, h;
	bool first = true;
	AVFrame *outFrame, *cpFrame;
	RawItem item;
	CPCMPlayer Aplayer;
	int readbytes, out_length;
	uint8_t *raw_pcm;
	CMediaReceiver receiver(DISABLE, sdpfilename);
	CMP3Decoder decoder;
	decoder.setupDecoder(2, 44100);
	Aplayer.setupPCMPlayer(2, 44100);

	if (receiver.getErrorState())
	{
		printf("%s", receiver.getErrorMsg().c_str());
		return;
	}
	receiver.start();
	while (!Aplayer.isStopped())
	{
		while (mthis->closed){ Sleep(100); }
		item = receiver.getAudioItem();
		if (!item.raw_data)
			break;
		do
		{
			ret = decoder.decodeMP3(item.raw_data, item.raw_length, &raw_pcm,
				out_length);
			if (ret > 0)
			{
				Aplayer.appendPCMData(raw_pcm, out_length);
			}

		} while (ret == 2);
	}
	receiver.terminate();
}
void video_handle(void *param)
{
	CStreamPlayer* mthis = (CStreamPlayer*)param;
	char* ip = mthis->tempip;
	int id = idcnt;

	char c = char(id + '0');
	char* sdpfilename = (char*)malloc(1000);
	sprintf(sdpfilename, "%c.sdp", c);
	

	CMediaSender MediaSender;
	MediaSender.setupProperties(ip, "disable", 640, 480);
	MediaSender.writeVideoSDPFile(sdpfilename);
	
	//char* hfilename = (char*)malloc(1000);
	//sprintf(hfilename, "%c.h264", c);
	//mthis->file.open(hfilename, ios::binary);
	

	int ret, w, h;
	bool first = true;
	AVFrame *yuvFrame, *rgbFrame;
	RawItem item;
	CImageFormatConverter cvt;

	CMediaReceiver receiver(sdpfilename, DISABLE);
	cvt.setupConverter(w, h, w, h, AV_PIX_FMT_YUV420P, AV_PIX_FMT_RGB24);
	CH264Decoder decoder;
	CImageIO image;
	decoder.setupDecoder();

	if (receiver.getErrorState())
	{
		printf("%s", receiver.getErrorMsg().c_str());
		return;
	}
	receiver.start();

	//bool fff = true;
	//mthis->qf = new QFile(hfilename);
	//mthis->qf->open(QIODevice::WriteOnly);
	//buf.open(QIODevice::ReadWrite);

	while (1)
	{
		while (mthis->closed){ Sleep(100); }
		item = receiver.getVideoItem(); 

		/*
		mthis->cnt++;
		if (mthis->meetend == false) buf.write((char*)item.raw_data, (qint64)item.raw_length);
		if (mthis->cnt == 900) mthis->meetend = true;
		if (mthis->meetend == true) {
			mthis->qf->write(buf.data(), buf.size()); mthis->qf->close();
		}
		*/

		mthis->nvs->updateMbps(item.raw_length);
		cout << "itemsize" << item.raw_length << endl;
		if (!item.raw_data)
			break;
		do
		{
			ret = decoder.decodeNalu(item.raw_data,
				item.raw_length, &yuvFrame,id);

			if (ret == -1)
				break;
			else if (ret)
			{
				if (first)
				{
					decoder.getSize(w, h);
					cvt.setupConverter(w, h, w, h, AV_PIX_FMT_YUV420P, AV_PIX_FMT_RGB24);
					first = false;
				}
				cvt.convert(yuvFrame->data[0], &rgbFrame);
				image.setupImageIO(w, h, (AVPixelFormat)rgbFrame->format, rgbFrame->data[0], rgbFrame->linesize[0] * h);
				if (image.getRawDataLength() == w * h * 3)
				{
					QImage *im = new QImage((uchar*)image.getRawData(), w, h, QImage::Format_RGB888);
					mthis->nvs->updateImage(*im);
					
					cout << "IDDDDDDDDDDDDDDDD" << id << endl;
				}

			}
		} while (ret == 2);
	
	}
}


CStreamPlayer::CStreamPlayer()
{
	layout = new QGridLayout(this);
	tempip = (char*)malloc(200);
	tempaip = (char*)malloc(200);
	//meetend = false;
}
void CStreamPlayer::setAddr(char* ip,char* aip)
{
	tempip = ip;
	tempaip = aip;
	idcnt++;

	nvs= new CVideoSurface(640, 480,false,false);
	layout->addWidget(nvs);


	Video_thread= new std::thread(video_handle, this);
	if(aip!="disable") Audio_thread= new std::thread(audio_handle, this);
}
CStreamPlayer::~CStreamPlayer()
{
	//meetend = true;
	//file.close();
}