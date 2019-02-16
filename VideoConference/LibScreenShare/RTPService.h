#ifndef RTPSERVICE_H
#define RTPSERVICE_H
#include "FFmpegBase.h"
#include "CPacketQueue.h"
#ifdef _MSC_VER
	#include <Windows.h> //only using Sleep
#elif __GNUC__
	#include <unistd.h>
	#define sleep(x) Sleep(x)
#endif
#define MEDIA_NONE_USED -1345
enum TRANSMISSION_TYPE
{
	MEDIA_NONE,
	MEDIA_H264_VIDEO = 100,
	MEDIA_MP3_AUDIO
};
typedef struct
{
	uint8_t *raw_data;
	int	     raw_length;
	TRANSMISSION_TYPE type;
}RawItem;
typedef struct
{
	int64_t s;
	int64_t drift;
	bool forceStop;
	bool first_play;
}TimeOut;
class CRTPSender : public CFFmpegCodecBase
{
public:
	CRTPSender();
	~CRTPSender();
	/*
		---If you call this constructor, you don't need to call setupEncoder function.---
	*/
	CRTPSender(const char *destIP, const int destPort, TRANSMISSION_TYPE type,
		float FPS, int audio_sample_rate, int video_width = 0, int video_height = 0);
	/*
		---Call this function before trying to encode anything.---
	*/
	bool						setupRTPSender(const char *destIP, const int destPort, TRANSMISSION_TYPE type,
												float FPS, int audio_sample_rate, int video_width, int video_height);
	/*
		---Important---
		Starting the transmission_handle() thread,
		Note that this function is only invoked once.
	*/
	void						start();
	/*
		---Common use---
		Use this function to terminate transmission_handle() thread.
		Once you terminate the RTP sender,
		that is, it's no longer to be used.
	*/
	void						finish();
	/*
		---Common use---
		Use this function to output sdp context into specified file.
	*/
	bool						writeSDPFile(const char *filename) const;
	/*
		---Common use---
		Use this function to pass raw_data (h264/mp3) to queue for being transmission.
	*/
	void						appendRawData(uint8_t *raw_data, int raw_length);
	/*
		---Common use---
		Use this function to get sdp context pointer from object.
	*/
	int							getItemCount();
	const char				   *getSDPcontext() const;
	void						showProperties() const final;
private:
	bool						internal_Init() final;
	void						transmission_handle();
	bool					    isInitializedRTPSender;
	bool						finished;
	std::thread				   *t1;
	std::string					destIP;
	int							destPort;
	int							frame_delay_ms;
	int							width;
	int							height;
	int							sample_rate;
	int64_t						frame_serial_num;
	char						sdp_context[10240];
	TRANSMISSION_TYPE			trans_type;
	CPacketQueue<AVPacket>		pkt_que;
	AVPacket					packet;
	AVStream				   *stream;
	AVFormatContext			   *pRtpFormat;
	AVCodecParserContext	   *pCodecParserCtx;
};
class CRTPReceiver : public CFFmpegCodecBase
{
public:
	CRTPReceiver();
	~CRTPReceiver();
	/*
		---If you call this constructor, you don't need to call setupEncoder function.---
	*/
	CRTPReceiver(const char *sdp_filename,int receiver_buffer_size = 30);
	/*
		---Call this function before trying to encode anything.---
	*/
	bool						setupRTPReceiver(const char *sdp_filename, int receiver_buffer_size = 30);
	/*
		---Important---
		Starting the transmission_handle() thread,
		Note that this function is only invoked once.
	*/
	void						start();
	/*
		---Common use---
		Pause the transmission_handle() thread,
		Using restart() for restarting.
	*/
	void						pause();
	/*
		---Common use---
		Restart the transmission_handle() thread
		that was stopped by you.
	*/
	void						restart();
	/*
		---Common use---
		Use this function to terminate transmission_handle() thread.
		Once you terminate the RTPReceiver
		that is, it's no longer to be used.
	*/
	void						finish();
	/*
		---Common use---
		Use this function to get the item that was put into the queue.
	*/
	RawItem						getItem(int itemlessthan = 0);
	/*
		---Common use---
		Use this function to number of items that were put into the queue.
	*/
	int							getItemCount();
	/*
		---Common use---
		Use this function to get the media type that receiver received from network.
	*/
	TRANSMISSION_TYPE			getMediaType();
	bool					    isNoError();
	void						showProperties() const final;
private:
	bool						internal_Init() final;
	void						transmission_handle();
	bool					    isInitializedRTPReceiver;
	bool						failure;
	bool						finished;
	bool						isStopped;
	char						sdp_filename[10240];
	int							time_drift;
	TimeOut						time_out;
	std::thread			       *t1;
	AVPacket					packet;
	AVStream				   *stream;
	AVFormatContext			   *pRtpFormat;
	CPacketQueue<AVPacket>		pkt_que;
	CPacketList<AVPacket>	   *item;
};
int interrupt_cb(void *ctx);
#endif