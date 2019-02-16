#include "Recorder.h"
bool CPaRecorder::isInitializedRecorder = false;
CPaRecorder::CPaRecorder()
{
	if (!isInitializedRecorder)
	{
		Pa_Initialize();
	}
	internal_init();
}
CPaRecorder::~CPaRecorder()
{
	finish();
	Pa_CloseStream(stream);
	Pa_Terminate();
	if (cb_smp_data.recordedSamples)
		av_free(cb_smp_data.recordedSamples);
}
CPaRecorder::CPaRecorder(int channels = 2, float record_seconds = 0.5, int nb_items = 10)
{
	if (!isInitializedRecorder)
	{
		Pa_Initialize();
	}
	internal_init();
	setupRecorder(channels, record_seconds, nb_items);
}
bool CPaRecorder::setupRecorder(int channels, float record_seconds,int nb_items)
{
	int numBytes;
	PaError err;
	this->recordSeconds  = record_seconds;
	this->channels		 = channels;
	
	microphoneIndex = Pa_GetDefaultInputDevice();
	if (microphoneIndex == paNoDevice)
	{
		av_log(0, AV_LOG_FATAL, "CPaRecorder:Could not get default input device.");
		return false;
	}
	microphoneInfo = Pa_GetDeviceInfo(microphoneIndex);
	if (!microphoneInfo)
	{
		av_log(0, AV_LOG_FATAL, "CPaRecorder:Could not get default input device info.");
		return false;
	}
	microphoneArgs.channelCount = channels;
	microphoneArgs.device = microphoneIndex;
	microphoneArgs.hostApiSpecificStreamInfo = NULL;
	microphoneArgs.sampleFormat = paInt16;
	microphoneArgs.suggestedLatency = microphoneInfo->defaultLowInputLatency;

	/*if (recordSeconds < microphoneArgs.suggestedLatency)
	{
		av_log(0, AV_LOG_FATAL, "CPaRecorder:Record second can't be less than input latency [%.3f].", 
			microphoneArgs.suggestedLatency);
		return false;
	}*/

	numBytes = (int) ( recordSeconds * microphoneInfo->defaultSampleRate * channels * sizeof(SAMPLE));

	cb_smp_data.sample_length = numBytes;
	cb_smp_data.recordedSamples = (SAMPLE*)av_malloc(numBytes);
	cb_smp_data.maxFrameIndex = (int)(recordSeconds * microphoneInfo->defaultSampleRate);
	cb_smp_data.frameIndex = 0;
	cb_smp_data.nextResult = paContinue;
	cb_smp_data.sample_rate = (int)microphoneInfo->defaultSampleRate;
	cb_smp_data.nb_channels = channels;

	int rec = (int)(record_seconds * microphoneInfo->defaultSampleRate);

	err = Pa_OpenStream(&stream, &microphoneArgs, NULL, microphoneInfo->defaultSampleRate,
		rec, 0, recordCallback, &cb_smp_data);
	
	//0.3 --> best match 64
	//0.2 --> best match 32
	//0.1 --> best match 16

	//output 16 samples

	SampleQue.setMaxSize(nb_items);

	if (err != paNoError)
	{
		av_log(0, AV_LOG_FATAL, "CPaRecorder:Could not open stream.");
		return false;
	}
	av_log(0, AV_LOG_INFO, "CPaRecorder is ready to start.\n");
	return true;
}
void CPaRecorder::start()
{
	if (!t1)
		t1 = new std::thread(&CPaRecorder::sample_handle, this);
	else
		av_log(0, AV_LOG_FATAL, "CPaRecorder: You can't start recorder twice!!!");
}
void CPaRecorder::stop()
{
	stopped = true;
}
void CPaRecorder::restart()
{
	stopped = false;
}
void CPaRecorder::finish()
{
	cb_smp_data.nextResult = paComplete;
	finished = true;
	SampleQue.break_getItem();
	if (t1)
	{
		t1->join();
		t1 = NULL;
	}
	while (item = SampleQue.getItem())
		delete item;
	SampleQue.reset();
}
SampleData CPaRecorder::getItem()
{
	SampleData out = { 0 };
	if (item)
		delete item;

	if (!finished)
	{
		item = SampleQue.getItem();
		if (item)
			out = item->data;
	}
	else
	{
		item = SampleQue.flushOutItem();
		if (item)
			out = item->data;
	}

	return out;
}
int CPaRecorder::getItemCount()
{
	return SampleQue.getSize();
}

int CPaRecorder::getSampleRate()
{
	return cb_smp_data.sample_rate;
}

void CPaRecorder::internal_init()
{	
	t1 = NULL;
	item = NULL;
	stream = NULL;
	microphoneInfo = NULL;
	microphoneIndex = 0;
	finished = false;
	stopped = false;
	memset(&cb_smp_data, 0, sizeof(cb_smp_data));
}
void CPaRecorder::sample_handle()
{
	PaError err;
	SampleData sample_data;
	sample_data.smp_length = cb_smp_data.sample_length;
	Pa_StartStream(stream);
	while ((err = Pa_IsStreamActive(stream)) == 1)
	{
		Pa_Sleep(10);
		if (stopped)
			cb_smp_data.frameIndex = 0;
		else if (finished)
			break;
		else if (cb_smp_data.frameIndex >= cb_smp_data.maxFrameIndex)
		{
			sample_data.samples = (SAMPLE *)av_malloc(cb_smp_data.sample_length);
			memcpy(sample_data.samples, cb_smp_data.recordedSamples,
				cb_smp_data.sample_length);
			SampleQue.push(sample_data);
			cb_smp_data.frameIndex = 0;
		}
	}
}
int recordCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	CallBackSampleData *data = (CallBackSampleData*)userData;
	const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
	SAMPLE *wptr = &data->recordedSamples[data->frameIndex * data->nb_channels];
	unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;
	int framesToCalc, i;

	if (framesLeft < framesPerBuffer)
		framesToCalc = framesLeft;
	else
		framesToCalc = framesPerBuffer;

	if (inputBuffer == NULL)
	{
		for (i = 0; i<framesToCalc; i++)
		{
			*wptr++ = 0;  /* left */
			if (data->nb_channels == 2) 
				*wptr++ = 0;  /* right */
		}
	}
	else
	{
		for (i = 0; i<framesToCalc; i++)
		{
			*wptr++ = *rptr++;  /* left */
			if (data->nb_channels == 2) 
				*wptr++ = *rptr++;  /* right */
		}
	}
	data->frameIndex += framesToCalc;
	return data->nextResult;
}