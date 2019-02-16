#ifndef RECORDER_H
#define RECORDER_H
#include "CPacketQueue.h"
#include "FFmpegBase.h"
#include <portaudio.h>
typedef short SAMPLE;
static int recordCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

typedef struct
{
	int          frameIndex;  /* Index into sample array. */
	int          maxFrameIndex;
	int	         sample_rate;
	int			 sample_length;
	int			 nb_channels;
	SAMPLE      *recordedSamples;
	PaStreamCallbackResult nextResult;
	std::mutex				mtx;
	std::condition_variable event;
} CallBackSampleData;

typedef struct
{
	int smp_length;
	SAMPLE *samples;
} SampleData;

class CPaRecorder
{
public:
	CPaRecorder();
	~CPaRecorder();
	CPaRecorder(int channels, float record_seconds, int nb_items);
	/*
		---Call this function before invoking starting function---	
	*/
	bool						 setupRecorder(int channels = 2,float record_seconds = 0.5,int nb_items = 10);
	/*
		---Important---
		Calling this function will start to record samples from microphone
		and create a thread (so this is a non-blocking function)
		the recorded samples would be put into SampleQue,
		you can use getItem function to get those samples.
	*/
	void						 start();
	/*
		---Note that---
		Calling this function will stop the recorder.
		Actually, that's `stop putting any samples to queue`.
	*/
	void						 stop();
	/*
		---Note that---
		If you have called stop function,
		you can invoke this function to recover recording mode.
	*/
	void						 restart();
	/*
		---Important---
		Calling this function when you are no longer to use recoder anymore.
		This function will disable all functions of this object.
	*/
	void						 finish();
	/*
		---Important---
		After you got sample data,
		you should call av_free to free the memory that sample used.
		
		As following code

		while(1)
		{
			data = recorder.getItem();
			...
			av_free(data.samples);
			...
		}
	*/
	SampleData					 getItem();
	/*
		---Common use---
		Use this function to get number of item.
	*/
	int							 getItemCount();
	/*
		---Common use---
		this function will return the sample rate of microphone.
	*/
	int							 getSampleRate();
private:
	void						 internal_init();
	void						 sample_handle();
	std::thread					*t1;
	CPacketQueue<SampleData>	 SampleQue;
	CPacketList<SampleData>		*item;
	PaStream					*stream;
	PaStreamParameters			 microphoneArgs;
	const PaDeviceInfo			*microphoneInfo;
	PaDeviceIndex				 microphoneIndex;
	CallBackSampleData			 cb_smp_data;
	static bool					 isInitializedRecorder;
	bool						 finished;
	bool					     stopped;
	float						 recordSeconds;
	int							 channels;
};
#endif