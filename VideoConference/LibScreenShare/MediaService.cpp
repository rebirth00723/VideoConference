#include "MediaService.h"
CMediaSender::CMediaSender()
{
	internal_Init();
}
CMediaSender::~CMediaSender()
{
	terminate();
}
CMediaSender::CMediaSender(std::string videoDestIPPort, std::string audioDestIPPort, int screenWidth,
	int screenHeight, VideoQuality vq, AudioQuality aq, bool cameraMode)
{
	internal_Init();
	setupProperties(videoDestIPPort, audioDestIPPort, screenWidth, screenHeight, vq, aq, cameraMode);
}
SetupResult CMediaSender::setupProperties(std::string videoDestIPPort, std::string audioDestIPPort,
	int screenWidth, int screenHeight, VideoQuality vq, AudioQuality aq, bool cameraMode)
{
	if (cameraMode)
		capturer = new CCameraCapturer();
	else
		capturer = new CScreenCapturer();
	disableCamera = !cameraMode;
	capturer->updateCapturerData();
	bool noerror(true);
	if (videoDestIPPort == DISABLE)
		disabledVideo = true;
	else if (!parseIPPort(videoDestIPPort, videoDestIP, videoDestPort))
	{
		printf("CMediaSender:Parsing [%s] failed, is the format really correct?\n",
			videoDestIPPort.c_str());
		hasError = true;
		lastErrorMsg = "CMediaSender:IP/Port parses failed";
		return SR_IPPORT_FORMAT_ERROR;
	}

	if (audioDestIPPort == DISABLE)
		disabledAudio = true;
	else if (!parseIPPort(audioDestIPPort, audioDestIP, audioDestPort))
	{
		printf("CMediaSender:Parsing [%s] failed, is the format really correct?\n",
			videoDestIPPort.c_str());
		hasError = true;
		lastErrorMsg = "CMediaSender:IP/Port parses failed";
		return SR_IPPORT_FORMAT_ERROR;
	}
	if (screenWidth == SCREEN_SIZE_AUTO || 
		screenHeight == SCREEN_SIZE_AUTO)
	{
		screenWidth  = capturer->getWidth();
		screenHeight = capturer->getHeight();
	}

	if (!disabledVideo)
	{
		switch (vq)
		{
		case VQ_LOW:
			hasError = !h264Encoder.setupEncoder(400000, screenWidth, screenHeight, 60, 15, 3, 0, 40, 50, X264_ENCODE_ULTRAFAST);
			break;
		case VQ_NORMAL:
			hasError = !h264Encoder.setupEncoder(400000, screenWidth, screenHeight, 60, 15, 3, 0, 30, 50, X264_ENCODE_ULTRAFAST);
			break;
		case VQ_HIGH:
			hasError = !h264Encoder.setupEncoder(600000, screenWidth, screenHeight, 60, 15, 3, 0, 20, 40, X264_ENCODE_ULTRAFAST);
			break;
		}

		if (hasError)
		{
			lastErrorMsg = "CMediaSender:h264Encoder set failed!";
			return SR_UNKNOWN_FAILED;
		}

		if (hasError = !imageConverter.setupConverter(capturer->getWidth(), capturer->getHeight(),
			screenWidth, screenHeight, AV_PIX_FMT_BGR24, AV_PIX_FMT_YUV420P))
		{
			lastErrorMsg = "CMediaSender:imageConverter set failed!";
			return SR_UNKNOWN_FAILED;
		}

		if (hasError = !h264RTPSender.setupRTPSender(videoDestIP.c_str(), videoDestPort,
			MEDIA_H264_VIDEO, 100, MEDIA_NONE_USED,
			screenWidth, screenHeight))
		{
			lastErrorMsg = "CMediaSender:h264RTPSender set failed!";
			return SR_UNKNOWN_FAILED;
		}
	}

	if (!disabledAudio)
	{
		if (!audioRec.setupRecorder())
		{
//			hasError = true;
//			lastErrorMsg = "CMediaSender:audioRec set failed!";
			disabledAudio = true;
			return SR_NO_MICROPHONE_DEVICE;
		}
		int smpr = audioRec.getSampleRate();

		switch (aq)
		{
		case AQ_LOW:
			hasError = !mp3Encoder.setupEncoder(AV_CH_LAYOUT_STEREO, 64000, smpr);
			break;
		case AQ_NORMAL:
			hasError = !mp3Encoder.setupEncoder(AV_CH_LAYOUT_STEREO, 128000, smpr);
			break;
		case AQ_HIGH:
			hasError = !mp3Encoder.setupEncoder(AV_CH_LAYOUT_STEREO, 256000, smpr);
			break;
		}

		if (hasError)
		{
			lastErrorMsg = "CMediaSender:mp3Encoder set failed!";
			return SR_UNKNOWN_FAILED;
		}

		if (hasError = !mp3RTPSender.setupRTPSender(audioDestIP.c_str(), audioDestPort,
			MEDIA_MP3_AUDIO, MEDIA_NONE_USED, smpr,
			MEDIA_NONE_USED, MEDIA_NONE_USED))
		{
			lastErrorMsg = "CMediaSender:h264RTPSender set failed!";
			return SR_UNKNOWN_FAILED;
		}
	}
	return SR_SUCCESS;
}
bool CMediaSender::start()
{
	closed = false;
	if (hasError)
		return false;
	if (!send_video_thread && !send_audio_thread)
	{
		if (!disabledVideo)
		{
			h264RTPSender.start();
			capturer->start();
			send_video_thread = new std::thread(&CMediaSender::send_video_handler, this);
		}
		if (!disabledAudio)
		{
			mp3RTPSender.start();
			audioRec.start();
			send_audio_thread = new std::thread(&CMediaSender::send_audio_handler, this);
		}
		return true;	
	}
	else
	{
		hasError = true;
		printf("CMediaSender:Can't start thread twice!\n");
		lastErrorMsg = "CMediaSender:Can't start thread twice!";
		return false;
	}
}
bool CMediaSender::hasStarted() const
{
	return send_video_thread || send_audio_thread;
}
void CMediaSender::terminate()
{
	closed = true;
	if (send_video_thread)
	{
		send_video_thread->join();
		delete send_video_thread;
		h264RTPSender.finish();
//		while (h264Encoder.flushFrameOut(&out_raw)){}
		h264Encoder.reset();
		capturer->finish();
	}
	if (send_audio_thread)
	{
		send_audio_thread->join();
		delete send_audio_thread;
		mp3RTPSender.finish();
		audioRec.finish();
	}		
	send_video_thread = nullptr;
	send_audio_thread = nullptr;
}
bool CMediaSender::writeVideoSDPFile(std::string path) const
{
	return h264RTPSender.writeSDPFile(path.c_str());
}
bool CMediaSender::writeAudioSDPFile(std::string path) const
{
	return mp3RTPSender.writeSDPFile(path.c_str());
}
std::string CMediaSender::getVideoSDPContent() const
{
	return h264RTPSender.getSDPcontext();
}
std::string CMediaSender::getAudioSDPContent() const
{
	return mp3RTPSender.getSDPcontext();
}
std::wstring CMediaSender::wgetVideoSDPContent() const
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return convertor.from_bytes(h264RTPSender.getSDPcontext());
}
std::wstring CMediaSender::wgetAudioSDPContent() const
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return convertor.from_bytes(mp3RTPSender.getSDPcontext());
}
bool  CMediaSender::getErrorState() const
{
	return hasError;
}
std::string  CMediaSender::getErrorMsg() const
{
	return lastErrorMsg;
}
void CMediaSender::send_video_handler()
{
	PicItem item;
	AVFrame *out;
	uint8_t *outyuv_raw;
	int ret;
	while (!closed)
	{
		item = capturer->getItem();
		if (disableCamera)
			imageConverter.inv_convert(item.pixels, &out);
		else
			imageConverter.convert(item.pixels, &out);
		do
		{
			ret = h264Encoder.encodeFragment(out->data[0], &outyuv_raw);
			if (ret)
			{
				h264RTPSender.appendRawData(outyuv_raw, ret);
			}
		} while (ret == 2);
		delete[] item.pixels;
	}
}
void CMediaSender::send_audio_handler()
{
	int i;
	int sample_rate;
	int bytes_left;
	int times;
	int buffer_size;
	int out_size;
	uint8_t *out_rawMP3;
	uint8_t *pass_samples;
	SampleData data;
	sample_rate = audioRec.getSampleRate();
	buffer_size = mp3Encoder.getBufferSize();
	while (!closed)
	{
		data = audioRec.getItem();
		if (!data.samples)
		{
			while ((out_size = mp3Encoder.flushFrameOut(&out_rawMP3)))
			{
				mp3RTPSender.appendRawData(out_rawMP3, out_size);
			}
			break;
		}
		bytes_left = data.smp_length % buffer_size;
		times = data.smp_length / buffer_size;
		pass_samples = (uint8_t*)data.samples;

		for (i = 0; i < times; ++i)
		{
			//samples is 2-byte format
			if ((out_size = mp3Encoder.encodeFragment(&(pass_samples[i * buffer_size]),
				buffer_size, &out_rawMP3)) > 0)
			{
				mp3RTPSender.appendRawData(out_rawMP3, out_size);
			}
		}
		if ((out_size = mp3Encoder.encodeFragment(&(pass_samples[i * buffer_size]),
			bytes_left, &out_rawMP3)) > 0)
		{
			mp3RTPSender.appendRawData(out_rawMP3, out_size);
		}
		av_free(data.samples);
	}
}
void CMediaSender::internal_Init()
{
	send_video_thread = nullptr;
	send_audio_thread = nullptr;
	closed = false;
	hasError = false;
	disabledVideo = false;
	disabledAudio = false;
	lastErrorMsg.clear();
}

CMediaReceiver::CMediaReceiver()
{
	internal_Init();
}
CMediaReceiver::~CMediaReceiver()
{
	terminate();
}
CMediaReceiver::CMediaReceiver(std::string videoSDPFilePath, std::string audioSDPFilePath)
{
	internal_Init();
	setSDPFilePath(videoSDPFilePath,audioSDPFilePath);
}
CMediaReceiver& CMediaReceiver::setRecordPath(std::string path)
{
	h264OutFile.close();
	mp3OutFile.close();
	outputPath = path;
	return *this;
}
bool CMediaReceiver::setSDPFilePath(std::string videoSDPFilePath, std::string audioSDPFilePath)
{
	this->videoSDPFilePath = videoSDPFilePath;
	this->audioSDPFilePath = audioSDPFilePath;
	if (videoSDPFilePath == DISABLE)
		disabledVideo = true;
	else if (!h264RTPReceiver.setupRTPReceiver(videoSDPFilePath.c_str()))
	{
		hasError = true;
		lastErrorMsg = "CMediaReceiver:h264RTPReceiver setupRTPReceiver failed.";
		return false;
	}
	if (audioSDPFilePath == DISABLE)
		disabledAudio = true;
	else if(!mp3RTPReceiver.setupRTPReceiver(audioSDPFilePath.c_str()))
	{
		hasError = true;
		lastErrorMsg = "CMediaReceiver:mp3RTPReceiver setupRTPReceiver failed.";
		return false;
	}
	return true;
}
bool CMediaReceiver::readSDPContent(std::string videoSDPContent, std::string audioSDPContent)
{
	std::ofstream vFilestream, aFilestream;
	std::string passV(DISABLE), passA(DISABLE);
	if (videoSDPContent.empty())
		videoSDPContent = DISABLE;
	if (audioSDPContent.empty())
		audioSDPContent = DISABLE;
	if (videoSDPContent == DISABLE)
		disabledVideo = true;
	else
	{
		vFilestream.open("v.sdp");
		if (!vFilestream.is_open())
		{
			hasError = true;
			lastErrorMsg = "CMediaReceiver:make v.sdp failed!";
			return false;
		}
		else
		{
			vFilestream << videoSDPContent;
			vFilestream.flush();
			vFilestream.close();
			passV = "v.sdp";
		}
	}
	if (audioSDPContent == DISABLE)
		disabledAudio = true;
	else
	{
		aFilestream.open("a.sdp");
		if (!aFilestream.is_open())
		{
			hasError = true;
			lastErrorMsg = "CMediaReceiver:make a.sdp failed!";
			return false;
		}
		else
		{
			aFilestream << audioSDPContent;
			aFilestream.flush();
			aFilestream.close();
			passA = "a.sdp";
		}
	}
	return setSDPFilePath(passV, passA);
}
bool CMediaReceiver::readSDPContent(std::wstring videoSDPContent, std::wstring audioSDPContent)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convertor;
	return readSDPContent(convertor.to_bytes(videoSDPContent), convertor.to_bytes(audioSDPContent));
}
CMediaReceiver&	CMediaReceiver::enableVideoRecord(bool newState)
{
	hasVideoRecord = newState;
	h264OutFile.open(outputPath + "recVideo.h264");
	if (!h264OutFile.is_open())
	{
		printf("Output [%s] recVideo.h264 failed!", outputPath.c_str());
		hasError = true;
		lastErrorMsg = "CMediaReceiver:Output recVideo.h264 failed!";
	}
	return *this;
}
CMediaReceiver& CMediaReceiver::enableAudioRecord(bool newState)
{
	hasAudioRecord = newState;
	mp3OutFile.open(outputPath + "recAudio.mp3");
	if (!mp3OutFile.is_open())
	{
		printf("Output [%s] recAudio.mp3 failed!", outputPath.c_str());
		hasError = true;
		lastErrorMsg = "CMediaReceiver:Output recAudio.mp3 failed!";
	}
	return *this;
}
RawItem CMediaReceiver::getVideoItem()
{
	RawItem item = h264RTPReceiver.getItem();
	if (hasVideoRecord && h264OutFile.is_open())
		h264OutFile.write((const char*)item.raw_data, item.raw_length);
	return item;
}
RawItem CMediaReceiver::getAudioItem()
{
	RawItem item = mp3RTPReceiver.getItem();
	if (hasAudioRecord && mp3OutFile.is_open())
		mp3OutFile.write((const char*)item.raw_data, item.raw_length);
	return item;
}
bool CMediaReceiver::start()
{
	closed = false;
	if (hasError || videoSDPFilePath.empty() || audioSDPFilePath.empty())
		return false;
	else
	{
		if (!disabledVideo)
			h264RTPReceiver.start();
		if (!disabledAudio)
			mp3RTPReceiver.start();
		return true;
	}
}
void CMediaReceiver::terminate()
{
	closed = true;
	if (!disabledVideo)
		h264RTPReceiver.finish();
	if (!disabledAudio)
		mp3RTPReceiver.finish();
}
bool CMediaReceiver::getErrorState() const
{
	return hasError;
}
std::string CMediaReceiver::getErrorMsg() const
{
	return lastErrorMsg;
}
void CMediaReceiver::internal_Init()
{
	recv_video_thread = nullptr;
	recv_audio_thread = nullptr;
	closed			  = false;
	hasVideoRecord    = false;
	hasAudioRecord    = false;
	hasError		  = false;
	disabledVideo     = false;
	disabledAudio	  = false;
	videoSDPFilePath.clear();
	audioSDPFilePath.clear();
	outputPath.clear();
	lastErrorMsg.clear();
}