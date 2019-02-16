#ifndef H264CODEC_H
#define H264CODEC_H
#include "FFmpegBase.h"
#include "nalu_defines.h"
extern "C"
{
#include <libswscale\swscale.h>
}
#define	X264_ENCODE_PLACEBO			"placebo"
#define	X264_ENCODE_VERYSLOW		"veryslow"
#define	X264_ENCODE_SLOWER			"slower"
#define	X264_ENCODE_SLOW			"slow"
#define	X264_ENCODE_MEDIUM			"medium"
#define	X264_ENCODE_FAST			"fast"
#define	X264_ENCODE_FASTER			"faster"
#define	X264_ENCODE_SUPERFAST		"superfast"
#define	X264_ENCODE_VERYFAST		"veryfast"
#define	X264_ENCODE_ULTRAFAST		"ultrafast"
class CH264Encoder : public CFFmpegCodecBase
{
public:
	CH264Encoder();
	~CH264Encoder();
	/*
		---If you call this constructor, you don't need to call setupEncoder function.---
	*/
	CH264Encoder(int64_t bit_rate,int width,int height,
		int FPS, int gop_size, int refs,
		int max_b_frames, int qmin, int qmax,
		const char *x264_encode_mode);
	/*
		---Call this function before trying to encode anything.---
	*/
    bool			setupEncoder(int64_t bit_rate, int width, int height,
						int FPS, int gop_size, int refs,
						int max_b_frames, int qmin, int qmax,
						const char *x264_encode_mode);
	void			showProperties() const final;
	bool			encodeFile(const char *inYUV420PFile, const char *outH264File);
	/*
		---Important---
		After using encodeFragment function, 
		you must call flushFrameOut to flush the remaining frame out.

		return value
		 -1 --> encoding error.
		  0 --> not enough data, providing more yuvPicture.
		  1 --> a encoded frame is complete.
	*/
	int				encodeFragment(uint8_t *yuvPicture, uint8_t **out_h264RawData);
	/*
		---Important---
		After using encodeFragment function, you must call this function to flush the remaining frame out.

		return value
		> 0   --> output portion of h264RawData, and the return value is the length of raw data;
		0 --> nothing could be flushed out.
	*/
	int				flushFrameOut(uint8_t **out_h264RawData);
	void			reset();
private:
	bool			internal_Init() final;
	AVPacket		packet;
	AVFrame		   *pFrame;
	uint8_t		   *pFrameDataBuf;
	uint32_t		yLen;
	uint32_t		uvLen;
	char		    xmode[15];
	bool			isInitializedH264Encoder;
};
class CH264Decoder : public CFFmpegCodecBase
{
public:
	CH264Decoder();
	~CH264Decoder();
	/*
		---Call this function before trying to decode anything.---	
	*/
	void						 setupDecoder();
	void						 showProperties() const final;
	/*
		---Easy to use---
		This function will flush frame automatically.	

		return value
		 0  --> decoding error
		 1  --> decoding has done.
	*/
	int							 decodeFile(const char *inH264File, const char *outYUV420PFile);
	/*
		---Easy to use---
		Specify a file and start to decode.
		This function will flush frame automatically.

		return value
		 -1  --> decoding error.
		  0  --> decoding has done.
		  1  --> a yuv420p frame is complete.
	*/
	int							 decodeFile(const char *inH264File, AVFrame **out_pFrame);
	/*
		---Note that---
		Calling this function, you have to pass h264 raw data(with Nalu)
		After decoding the frame, you have to flush the remaining frame out.
		For an instance, opening a h264 file and fread some byte to pass to this.
		---Warning---
		This function will miss the last frame, if you just want to decode file don't use this.

		return value
		 -1  --> decoding error.
		  0  --> not enough data, providing more h264 raw data.
		  1  --> a encoded frame is complete.
		  2  --> hold the raw data and call decodeNalu to get more frame.
	 */
	int							 decodeNalu(uint8_t *h264_data,size_t dataLength, AVFrame **out_frame,int id);//Nei
	/*
		---Note that---
		Calling this function, you have to pass h264 Nalu.
		After decoding the frame, you have to flush the remaining frame out.
		---Warning---
		This function will miss the last frame, if you just want to decode file don't use this.

		return value
		 -1  --> decoding error.
		  0  --> not enough data, providing more yuvPicture.
		  1  --> a encoded frame is complete.
		  2  --> hold the raw data and call decodeNalu to get more frame..
	*/
	int							 decodeNalu(nalu_t *nalu, AVFrame **out_frame);
	/*
		---Important---
		After using decodeNalu function, you must call this function to flush the remaining frame out.
		
		return value
		 true  --> get a new yuv420p frame.
		 false --> nothing could be flushed out.
	*/
	bool						 flushFrameOut(AVFrame **out_frame);
	/*
		---Important---
		If you need to reset the decoder,
		you must call this function.
	*/
	void						 decodeNaluReset();
	/*
		---Important---
		Because the decode function outputs frames allocated with same memory,
		you need to copy a frame for other using.
	*/
	AVFrame						*copy_yuvFrame(const AVFrame *yuvPic);
	/*
		---Important---
		free the frame that you got with copy yuvframe function.	
	*/
	static void					 free_yuvFrame(AVFrame **yuvPic);
private:
	bool						 internal_Init() final;
	AVPacket					 packet;
	AVFrame						*pFrame;
	AVFrame						*pFrameOut;
	uint8_t						*pFrameOutBuf;
	int						 	 cur_size; //for decodeNalu
	bool						 hasSwsCtx;
	uint32_t					 yLen;
	uint32_t					 uvLen;
	AVCodecParserContext		*pH264DecoderParserCtx;
	SwsContext				    *pSwsCtx;
	bool						 isInitializedH264Decoder;
};
#endif