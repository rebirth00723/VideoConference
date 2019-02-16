#ifndef MP3_CODEC_H
#define MP3_CODEC_H
#include "FFmpegBase.h"
extern "C"
{
#include <libswresample/swresample.h>
}
class CMP3Encoder : CFFmpegCodecBase
{
public:
	CMP3Encoder();
	~CMP3Encoder();
	/*
		---If you call this constructor, you don't need to call setupEncoder function.---
	*/
	CMP3Encoder(uint64_t channel_layout, int bit_rate, int sample_rate);
	/*
		---Call this function before trying to encode anything.---
	*/
	bool				 setupEncoder(uint64_t channel_layout, int bit_rate, int sample_rate);
	bool				 encodeFile(const char *inPCM_2CH_2BYTES_File, const char *outMP3File);
	/*
		---Important---
		Your raw_length can't exceed the size of buffer, knowing the buffer size you should call getBufferSize method.
		After using encodeFragment function, you must call flushFrameOut to flush the remaining frame out.
		Note that, the raw_length should be less than buffer size (invoke getBufferSize() for getting)

		return value
		-1 --> encoding error.
		0  --> not enough data, providing more raw PCM data.
		>0 --> output portion of h264RawData, and the return value is the length of raw data;
	*/
	int					 encodeFragment(uint8_t *rawPCM_2CH_2BYTES, size_t raw_length, uint8_t **out_MP3RawData);
	/*
		---Important---
		After using encodeFragment function, you must call this function to flush the remaining frame out.

		return value
		>0  --> output portion of h264RawData, and the return value is the length of raw data;
		0   --> nothing could be flushed out.
	*/
	int					 flushFrameOut(uint8_t **out_MP3RawData);
	void				 showProperties() const final;
	/*
		---Usage---
		Use this function to determine the size of buffer.

		return value
		>0 --> the size of buffer.
		0  --> haven't initialized the encoder.
	*/
	size_t				 getBufferSize() const;
private:
	const int		     MAX_OUT_BUF_SIZE = 192000;
	AVFrame				*pFrame;
	uint16_t			*pFrameDataBuf;
	size_t				 bufferSize;
	AVPacket			 packet;
	SwrContext			*pSwrCtx;
	bool			     internal_Init();
	bool				 isInitializedMP3Encoder;
private:
};
class CMP3Decoder : CFFmpegCodecBase
{
public:
	CMP3Decoder();
	~CMP3Decoder();
	/*
		---If you call this constructor, you don't need to call setupDecoder function.---
	*/
	CMP3Decoder(int out_channels, int out_sample_rate);
	/*
		---Call this function before trying to decode anything.---
	*/
	void					  setupDecoder(int out_channels, int out_sample_rate);
	void					  showProperties() const final;
	/*
		---Easy to use---
		Specify in/output file and start to decode.

		return value
		0  --> decoding error
		1  --> decoding has done.
	*/
	int						  decodeFile(const char *inMP3File, const char *outPCMFile);
	/*
		---Easy to use---
		Specify input file and start to decode.

		return value
		-1  --> decoding error.
		0  --> decoding has done.
		1  --> a outPCMData frame is complete.
	*/
	int						  decodeFile(const char *inMP3File, uint8_t **out_PCMData, int &out_PCMDataLength);
		/*
		---Note that---
		Calling this function, you have to pass mp3 raw data.
		For an instance, opening a h264 file and fread some byte to pass to this.
		MP3 don't need to flush any frame.
		After decoding has done, you must call decodeMP3Close() method.

		return value
		-1 --> decoding error.
		0  --> not enough data, providing more mp3 raw data.
		1  --> a decoded frame is complete.
		2  --> hold the raw data and call decodeMP3 to get more frame.
	*/
	int						  decodeMP3(uint8_t *rawMP3data, int dataLength, uint8_t **out_PCMData, int &out_PCMDataLength);
	/*
		---Important---
		After decodeMP3 method has end, you must need to call this function.
	*/
	void					  decodeMP3Close();
	/*
		Reset the decodeMP3 function.
	*/
	void					  decodeMP3Reset();
private:
	bool					  internal_Init() final;
	const int				  MAX_OUT_BUF_SIZE = 192000;
	struct SwrContext		 *pSwrCtx;
	AVCodecParserContext	 *pCodecParserCtx;
	AVPacket				  packet;
	AVFrame					 *pFrame;
	bool					  hasSwrCtx;
	bool					  isInitializedMP3Decoder;
	int						  out_channels;
	int						  out_sample_rate;
	int						  cur_size;
	uint8_t				     *out_buf;
};
#endif
