#ifndef MEDIASERVICE_H
#define MEDIASERVICE_H
#include "NetworkUtils.h"
#include "RTPService.h"
#include "ScreenCapturer.h"
#include "CameraCapturer.h"
#include "Recorder.h"
#include "H264Codec.h"
#include "MP3Codec.h"
#include "ImageUtils.h"
#define DISABLE			 "disable"
#define SCREEN_SIZE_AUTO 2147483647
typedef enum
{
	VQ_LOW,
	VQ_NORMAL,
	VQ_HIGH
}VideoQuality;
typedef enum
{
	AQ_LOW,    //11025Hz
	AQ_NORMAL, //22050Hz
	AQ_HIGH    //44100Hz
}AudioQuality;
typedef enum
{
	SR_SUCCESS,
	SR_IPPORT_FORMAT_ERROR,
	SR_UNKNOWN_FAILED,
	SR_NO_MICROPHONE_DEVICE
}SetupResult;
class CMediaSender
{
public:
	CMediaSender();
	~CMediaSender();
	CMediaSender(std::string videoDestIPPort , std::string audioDestIPPort ,
				int screenWidth = SCREEN_SIZE_AUTO, int screenHeight = SCREEN_SIZE_AUTO, 
				VideoQuality vq = VQ_NORMAL, AudioQuality aq = AQ_NORMAL, bool cameraMode = false);
	/*
		return value
		true  == successful
		false == failed
	*/
	void setupIPPORT(std::string videoDestIPPort, std::string audioDestIPPort)
	{
		if (videoDestIPPort == DISABLE)
			disabledVideo = true;
		else if (!parseIPPort(videoDestIPPort, videoDestIP, videoDestPort))
		{
			printf("CMediaSender:Parsing [%s] failed, is the format really correct?\n",
				videoDestIPPort.c_str());
			hasError = true;
			lastErrorMsg = "CMediaSender:IP/Port parses failed";
		}

		if (audioDestIPPort == DISABLE)
			disabledAudio = true;
		else if (!parseIPPort(audioDestIPPort, audioDestIP, audioDestPort))
		{
			printf("CMediaSender:Parsing [%s] failed, is the format really correct?\n",
				videoDestIPPort.c_str());
			hasError = true;
			lastErrorMsg = "CMediaSender:IP/Port parses failed";
		}
	};
	SetupResult						setupProperties(std::string videoDestIPPort, std::string audioDestIPPort,
										int screenWidth = SCREEN_SIZE_AUTO, int screenHeight = SCREEN_SIZE_AUTO,
										VideoQuality vq = VQ_NORMAL, AudioQuality aq = AQ_NORMAL, bool cameraMode = false);
	bool							start();
	bool							hasStarted() const;
	void							terminate();
	bool							writeVideoSDPFile(std::string path) const;
	bool							writeAudioSDPFile(std::string path) const;
	std::string						getVideoSDPContent() const;
	std::string						getAudioSDPContent() const;
	std::wstring					wgetVideoSDPContent() const;
	std::wstring					wgetAudioSDPContent() const;
	bool							getErrorState() const;
	std::string						getErrorMsg() const;
//private:
	bool							closed;
	bool							hasError;
	bool							disableCamera;
	bool							disabledVideo;
	bool							disabledAudio;
	std::string						lastErrorMsg;
	std::thread					   *send_video_thread;
	std::thread					   *send_audio_thread;
	void							send_video_handler();

	void							send_audio_handler();
	void							internal_Init();
	std::string						videoDestIP;
	int								videoDestPort;
	std::string						audioDestIP;
	int								audioDestPort;
	CH264Encoder					h264Encoder;
	CMP3Encoder						mp3Encoder;
	CImageFormatConverter			imageConverter;
	CAbstractCapturer*				capturer;
	CPaRecorder						audioRec;
	CRTPSender						h264RTPSender;
	CRTPSender						mp3RTPSender;
};
class CMediaReceiver
{
public:
	CMediaReceiver();
	~CMediaReceiver();
	CMediaReceiver(std::string videoSDPFilePath, std::string audioSDPFilePath);

	CMediaReceiver&				setRecordPath(std::string path);
	/*
		The IP description of SDP file must be multicast IP,
		If not, it will occur binding failed.
		refer to source code https://ffmpeg.org/doxygen/trunk/udp_8c-source.html
	*/
	bool						setSDPFilePath(std::string videoSDPFilePath, std::string audioSDPFilePath);
	bool						readSDPContent(std::string videoSDPContent, std::string audioSDPContent);
	bool						readSDPContent(std::wstring videoSDPContent, std::wstring audioSDPContent);
	CMediaReceiver&				enableVideoRecord(bool newState);
	CMediaReceiver&				enableAudioRecord(bool newState);
	RawItem						getVideoItem();
	RawItem						getAudioItem();
	bool						start();
	void						terminate();
	bool						getErrorState() const;
	std::string					getErrorMsg() const;
private:
	bool						hasError;
	std::ofstream				h264OutFile;
	std::ofstream				mp3OutFile;
	bool						closed;
	bool						disabledVideo;
	bool						disabledAudio;
	bool						hasVideoRecord;
	bool						hasAudioRecord;
	void						internal_Init();
	std::thread		           *recv_video_thread;
	std::thread				   *recv_audio_thread;
	std::string					lastErrorMsg;
	std::string					videoSDPFilePath;
	std::string					audioSDPFilePath;
	std::string					outputPath;
	CRTPReceiver				h264RTPReceiver;
	CRTPReceiver				mp3RTPReceiver;
};
#endif