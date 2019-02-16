#include "H264Codec.h"
CH264Encoder::CH264Encoder()
{	
	isInitializedH264Encoder = false;
	internal_Init();
}
CH264Encoder::CH264Encoder(
	int64_t bit_rate, int width, int height,
	int FPS, int gop_size, int refs, int max_b_frames,
	int qmin, int qmax, const char *x264_encode_mode)
{
	isInitializedH264Encoder = false;
	internal_Init();
	setupEncoder(bit_rate, width, height, FPS,
		gop_size, refs, max_b_frames, qmin, qmax, 
		x264_encode_mode);
}
CH264Encoder::~CH264Encoder()
{
	avcodec_free_context(&pCodecCtx);
	av_packet_unref(&packet);
	if (pFrame)
		av_frame_free(&pFrame);
	if (pFrameDataBuf)
		av_free(pFrameDataBuf);

}
bool CH264Encoder::internal_Init()
{
	if (!isInitializedH264Encoder)
	{
		pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!pCodec)
		{
			av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not find the H264 encoder.\n");
			return false;
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx)
		{
			av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not allocate codec context for H264 encoder.\n");
			return false;
		}
		isInitializedH264Encoder = true;
		memset(xmode, 0, sizeof(xmode));
		memset(&packet, 0, sizeof(AVPacket));
		yLen = uvLen = 0;
		pFrame = NULL;
		av_log(0, AV_LOG_INFO, "CH264Encoder has been initialized.\n");
	}
	return true;
}
bool CH264Encoder::setupEncoder(int64_t bit_rate, int width, int height,
	int FPS, int gop_size,int refs,
	int max_b_frames, int qmin, int qmax,
	const char *x264_encode_mode)
{
	uint32_t pFrameSize;
	pCodecCtx->bit_rate = width * height * 4;
	pCodecCtx->width = width;
	pCodecCtx->height = height;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = FPS;
	pCodecCtx->framerate.num = FPS;
	pCodecCtx->framerate.den = 1;

	pCodecCtx->gop_size = gop_size;
	pCodecCtx->max_b_frames = max_b_frames;
	pCodecCtx->qmin = qmin;
	pCodecCtx->qmax = qmax;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->refs = refs;
	strcpy(xmode, x264_encode_mode);
	
	if (av_opt_set(pCodecCtx->priv_data,
		"preset", x264_encode_mode, 0) != 0)
	{
		av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not set x264 encoding mode.\n");
		return false;
	}
	
	/*Initialize frame*/
	pFrame = av_frame_alloc();
	if (!pFrame)
	{
		av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not allocate video frame.\n");
		return false;
	}
	av_init_packet(&packet);
	/*Initialize frame data*/
	pFrame->pts = 0;
	pFrame->format = AV_PIX_FMT_YUV420P;
	pFrame->width = pCodecCtx->width;
	pFrame->height = pCodecCtx->height;
	pFrameSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
		pCodecCtx->width, pCodecCtx->height,1);
	pFrameDataBuf = (uint8_t*)av_mallocz(pFrameSize);
	av_image_fill_arrays(pFrame->data, pFrame->linesize, pFrameDataBuf, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
	yLen = pCodecCtx->width * pCodecCtx->height;
	uvLen = yLen >> 2;
	if (!(pFrame && pFrameDataBuf))
	{
		av_log(0,AV_LOG_ERROR, "CH264Encoder:Could not allocate memory to frame.\n");
		return false;
	}
	av_log(0, AV_LOG_INFO, "CH264Encoder has been set-up successfully.\n");
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not open CH264Encoder.\n");
		return false;
	}
	av_log(0, AV_LOG_INFO, "CH264Encoder codec has been opened, and being able to work.\n");
	return true;
}
bool CH264Encoder::encodeFile(const char *inYUV420PFile, const char *outH264File)
{
	std::ifstream inFile(inYUV420PFile, std::ios::binary | std::ios::in);
	std::ofstream outFile(outH264File, std::ios::binary | std::ios::out);
	int ret,got_packet_ptr;
	bool quit;
	if (!inFile.is_open())
	{
		av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not open the file from [%s]\n", inYUV420PFile);
		return false;
	}
	if (!outFile.is_open())
	{
		av_log(0, AV_LOG_ERROR, "CH264Encoder:Could not write the file to [%s]\n", outH264File);
		inFile.close();
		return false;
	}
	quit = false;
	av_log(0, AV_LOG_INFO, "CH264Encoder:Input file path:[%s]\n", inYUV420PFile);
	av_log(0, AV_LOG_INFO, "CH264Encoder:Output file path:[%s]\n", outH264File);
	av_log(0, AV_LOG_INFO, "CH264Encoder:Start encoding...");
	while (1)
	{
		av_init_packet(&packet);
		packet.data = NULL;
		packet.size = 0;
		inFile.read((char*)pFrame->data[0], yLen);
		inFile.read((char*)pFrame->data[1], uvLen);
		inFile.read((char*)pFrame->data[2], uvLen);
		pFrame->pts++;
		if (inFile.eof())
			quit = true;

		ret = avcodec_encode_video2(pCodecCtx, &packet, pFrame, &got_packet_ptr);
		if (ret < 0)
		{
			printf("CH264Encoder:Error encoding frame\n");
			return false;
		}
		if (got_packet_ptr)
		{
			outFile.write((char*)packet.data, packet.size);
			av_packet_unref(&packet);
		}
		if (quit)
			break;
	}
	
	for (got_packet_ptr = 1; got_packet_ptr;)
	{
		ret = avcodec_encode_video2(pCodecCtx, &packet, NULL, &got_packet_ptr);
		if (ret < 0) 
		{
			printf("CH264Encoder:Error encoding frame\n");
			return false;
		}
		if (got_packet_ptr) 
		{
			outFile.write((char*)packet.data, packet.size);
			av_packet_unref(&packet);
		}
	}
	av_log(0, AV_LOG_INFO, "\nCH264Encoder:Encoding has been done.\n");
	outFile.close();
	inFile.close();
	return true;
}
int CH264Encoder::encodeFragment(uint8_t *yuvPicture, uint8_t **out_h264RawData)
{
	int got_packet_ptr;
	static bool hasOut;
	pFrame->data[0] = yuvPicture;
	pFrame->data[1] = yuvPicture + yLen;
	pFrame->data[2] = yuvPicture + yLen + uvLen;
	if (hasOut)
	{
		hasOut = false;
		av_packet_unref(&packet);
	}
	if (avcodec_encode_video2(pCodecCtx, &packet, pFrame, &got_packet_ptr) >= 0)
	{
		pFrame->pts++;
		if (got_packet_ptr)
		{
			hasOut = true;
			*out_h264RawData = packet.data;
			return packet.size;
		}
		return 0;
	}
	else
		return -1;
}
int CH264Encoder::flushFrameOut(uint8_t **out_h264RawData)
{
	int got_packet_ptr = 1, ret;
	ret = avcodec_encode_video2(pCodecCtx, &packet, NULL, &got_packet_ptr);
	if (ret < 0)
	{
		printf("CH264Encoder:Error encoding frame\n");
		return 0;
	}
	if (got_packet_ptr)
	{
		*out_h264RawData = packet.data;
		return packet.size;
	}
	else
		return 0;
}
void CH264Encoder::reset()
{
	AVCodecContext p = *pCodecCtx;
	avcodec_free_context(&pCodecCtx);
	av_packet_unref(&packet);
	if (pFrame)
		av_frame_free(&pFrame);
	if (pFrameDataBuf)
		av_free(pFrameDataBuf);

	isInitializedH264Encoder = false;
	internal_Init();

	setupEncoder(0, p.width, p.height, p.time_base.den,
		p.gop_size, p.refs, p.max_b_frames, p.qmin, p.qmax, xmode
		);
}
void CH264Encoder::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----H264 Encoder Information----\n");	
	av_log(0, AV_LOG_INFO, "\tBitrate:%d\n", pCodecCtx->bit_rate);
	av_log(0, AV_LOG_INFO, "\tWidth:%d\n", pCodecCtx->width);
	av_log(0, AV_LOG_INFO, "\tHeight:%d\n", pCodecCtx->height);
	av_log(0, AV_LOG_INFO, "\tFPS:%d\n", pCodecCtx->time_base.den);
	av_log(0, AV_LOG_INFO, "\tGop size:%d\n", pCodecCtx->gop_size);
	av_log(0, AV_LOG_INFO, "\tMax b frames:%d\n", pCodecCtx->max_b_frames);
	av_log(0, AV_LOG_INFO, "\tQmin:%d\n", pCodecCtx->qmin);
	av_log(0, AV_LOG_INFO, "\tQmax:%d\n", pCodecCtx->qmax);
	av_log(0, AV_LOG_INFO, "\tFormat:YUV420P\n");
	av_log(0, AV_LOG_INFO, "\tMode:%s\n", xmode);	

}
//H264 decoder
CH264Decoder::CH264Decoder()
{
	isInitializedH264Decoder = false;
	internal_Init();
}
CH264Decoder::~CH264Decoder()
{
	avcodec_free_context(&pCodecCtx);
	av_parser_close(pH264DecoderParserCtx);
	av_packet_unref(&packet);
	sws_freeContext(pSwsCtx);
	if (pFrame)
		av_frame_free(&pFrame);
	if (pFrameOut)
		av_frame_free(&pFrameOut);
	if (pFrameOutBuf)
		av_freep(&pFrameOutBuf);
	
}
void CH264Decoder::setupDecoder()
{
	pFrame = av_frame_alloc();
	pFrameOut = av_frame_alloc();

	av_init_packet(&packet);

	if (pCodec->capabilities&CODEC_CAP_TRUNCATED)
		pCodecCtx->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec\n");
		return;
	}
	av_log(0, AV_LOG_INFO, "CH264decoder codec has been opened, and being able to work.\n");
}
void CH264Decoder::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----H264 decoder Information----\n");
	av_log(0, AV_LOG_INFO, "\tBitrate:%d\n", pCodecCtx->bit_rate);
	av_log(0, AV_LOG_INFO, "\tWidth:%d\n", pCodecCtx->width);
	av_log(0, AV_LOG_INFO, "\tHeight:%d\n", pCodecCtx->height);
	av_log(0, AV_LOG_INFO, "\tFPS:%d\n", pCodecCtx->time_base.den);
	av_log(0, AV_LOG_INFO, "\tGop size:%d\n", pCodecCtx->gop_size);
	av_log(0, AV_LOG_INFO, "\tMax b frames:%d\n", pCodecCtx->max_b_frames);
	av_log(0, AV_LOG_INFO, "\tQmin:%d\n", pCodecCtx->qmin);
	av_log(0, AV_LOG_INFO, "\tQmax:%d\n", pCodecCtx->qmax);
	av_log(0, AV_LOG_INFO, "\tFormat:YUV420P\n");
}
int CH264Decoder::decodeFile(const char *inH264File, const char *outYUV420PFile)
{
	std::ofstream outFile(outYUV420PFile, std::ios::binary | std::ios::out);
	AVFormatContext *pFormatCtx = NULL;
	int got_picture, ret;
	int y_size=0;
	if (avformat_open_input(&pFormatCtx, inH264File, 0, 0) != 0)
	{
		av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not open the file from [%s]\n", inH264File);
		return 0;
	}
	if (avformat_find_stream_info(pFormatCtx, 0) < 0)
	{
		av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not find the stream info from [%s]\n", inH264File);
		return 0;
	}
	av_dump_format(pFormatCtx, 0, inH264File, 0);
	if (!outFile.is_open())
	{
		av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not write the file to [%s]\n", outYUV420PFile);
		return 0;
	}
	av_log(0, AV_LOG_INFO, "CH264Decoder:Input file path:[%s]\n", inH264File);
	av_log(0, AV_LOG_INFO, "CH264Decoder:Output file path:[%s]\n", outYUV420PFile);
	av_log(0, AV_LOG_INFO, "CH264Decoder:Start decoding...");
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{		
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (!y_size)
			y_size = pFrame->width * pFrame->height;
		if (ret < 0)
		{
			printf("Decode Error.\n");
			return 0;
		}
		if (got_picture)
		{
			if (!hasSwsCtx)
			{
				hasSwsCtx = true;
				pSwsCtx = sws_getCachedContext(pSwsCtx, pCodecCtx->width,
					pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height,
					AV_PIX_FMT_YUV420P, SWS_BILINEAR, 0, 0, 0);
				ret = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
				pFrameOutBuf = (uint8_t*)av_malloc(ret);
				av_image_fill_arrays(pFrameOut->data, pFrameOut->linesize,
					pFrameOutBuf, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height,1);
			}
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize
				);
			outFile.write((char*)pFrameOut->data[0], y_size);
			outFile.write((char*)pFrameOut->data[1], y_size >> 2);
			outFile.write((char*)pFrameOut->data[2], y_size >> 2);
			av_packet_unref(&packet);
		}
	}
	packet.data = NULL;
	packet.size = 0;
	while (1)
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet); 
		if (ret < 0)
		{
			printf("CH264Decoder:Decode Error.\n");
			return 0;
		}
		if (!got_picture)
			break;
		if (got_picture)
		{
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize
				);
			outFile.write((char*)pFrameOut->data[0], y_size);
			outFile.write((char*)pFrameOut->data[1], y_size >> 2);
			outFile.write((char*)pFrameOut->data[2], y_size >> 2);
			av_packet_unref(&packet);
		}
	}
	av_log(0, AV_LOG_INFO, "\nCH264Decoder:Decoding has been done.\n");
	hasSwsCtx = false;
	sws_freeContext(pSwsCtx);
	pSwsCtx = sws_alloc_context();
	avformat_close_input(&pFormatCtx);
	outFile.close();
	return 1;
}
int CH264Decoder::decodeFile(const char *inH264File, AVFrame **out_pFrame)
{
	static AVFormatContext *pFormatCtx;
	int got_picture, ret;
	if (!pFormatCtx)
	{
		if (avformat_open_input(&pFormatCtx, inH264File, 0, 0) != 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not open the file from [%s]\n", inH264File);
			return -1;
		}
		if (avformat_find_stream_info(pFormatCtx, 0) < 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not find the stream info from [%s]\n", inH264File);
			return -1;
		}
		av_dump_format(pFormatCtx, 0, inH264File, 0);
	}
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Decode Error.\n");
			return -1;
		}
		if (got_picture)
		{
			if (!hasSwsCtx)
			{
				hasSwsCtx = true;
				pSwsCtx = sws_getCachedContext(pSwsCtx, pCodecCtx->width,
					pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height,
					AV_PIX_FMT_YUV420P, SWS_BILINEAR, 0, 0, 0);
				ret = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
				pFrameOutBuf = (uint8_t*)av_malloc(ret);
				av_image_fill_arrays(pFrameOut->data, pFrameOut->linesize,
					pFrameOutBuf, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
			}
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize);
			*out_pFrame = pFrameOut;
			av_packet_unref(&packet);
			return 1;
		}
	}
	while (1)
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Decode Error.\n");
			return -1;
		}
		if (!got_picture)
		{
			avformat_close_input(&pFormatCtx);
			hasSwsCtx = false;
			sws_freeContext(pSwsCtx);
			pSwsCtx = sws_alloc_context();
			avformat_close_input(&pFormatCtx);
			return 0;
		}
		if (got_picture)
		{
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize);
			*out_pFrame = pFrameOut;
			av_packet_unref(&packet);
			return 1;
		}
	}
}

int CH264Decoder::decodeNalu(uint8_t *h264_data, size_t dataLength, AVFrame **out_frame,int id)
{
	int got_picture, ret;
	static uint8_t *cur_ptr[100];
	if (!cur_size)
	{
		cur_ptr[id] = h264_data;
		cur_size = dataLength;
	}
	while (cur_size > 0)
	{
		int len = av_parser_parse2(
			pH264DecoderParserCtx, pCodecCtx,
			&packet.data, &packet.size,
			cur_ptr[id], cur_size,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		cur_ptr[id] += len;
		cur_size -= len;
		if (packet.size == 0)
			continue;
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Decode Error.\n");
			decodeNaluReset();
			return -1;
		}
		if (got_picture)
		{
			if (!hasSwsCtx)
			{
				hasSwsCtx = true;
				pSwsCtx = sws_getCachedContext(pSwsCtx, pCodecCtx->width,
					pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height,
					AV_PIX_FMT_YUV420P, SWS_BILINEAR, 0, 0, 0);
				ret = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
				pFrameOutBuf = (uint8_t*)av_malloc(ret);
				av_image_fill_arrays(pFrameOut->data, pFrameOut->linesize,
					pFrameOutBuf, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
			}
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize);
			*out_frame = pFrameOut;
			if (cur_size)
				return 2;
			else
				return 1;
		}
	}
	return 0;
}
int CH264Decoder::decodeNalu(nalu_t *nalu, AVFrame **out_frame)
{
	int got_picture, ret;
	int y_size = 0;
	AVPacket packet;
	uint8_t *cur_ptr;
	static uint8_t in_buffer[500000];

	addstartcode_prefix((char*)in_buffer, nalu->startcodeprefix_len);
	memcpy(in_buffer + nalu->startcodeprefix_len, nalu->buf, nalu->len);
	cur_ptr = in_buffer;
	cur_size = nalu->len + nalu->startcodeprefix_len;
	
	while (cur_size > 0)
	{
		int len = av_parser_parse2(
			pH264DecoderParserCtx, pCodecCtx,
			&packet.data, &packet.size,
			cur_ptr, cur_size,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		cur_ptr += len;
		cur_size -= len;
		if (packet.size == 0)
			continue;

		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Decode Error.\n");
			return -1;
		}
		if (got_picture)
		{
			if (!hasSwsCtx)
			{
				hasSwsCtx = true;
				pSwsCtx = sws_getCachedContext(pSwsCtx, pCodecCtx->width,
					pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height,
					AV_PIX_FMT_YUV420P, SWS_BILINEAR, 0, 0, 0);
				ret = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
				pFrameOutBuf = (uint8_t*)av_malloc(ret);
				av_image_fill_arrays(pFrameOut->data, pFrameOut->linesize,
					pFrameOutBuf, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
			}
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize);
			*out_frame = pFrameOut;
			if (cur_size)
				return 2;
			else
				return 1;
			return 1;
		}
	}
	return 0;
}
void CH264Decoder::decodeNaluReset()
{
	AVFrame *pFrame;
	cur_size = 0;
	while (flushFrameOut(&pFrame));
}
bool CH264Decoder::flushFrameOut(AVFrame **out_frame)
{
	int ret;
	int got_picture;
	packet.data = NULL;
	packet.size = 0;
	while (1)
	{
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Decode Error.\n");
			return false;
		}
		if (!got_picture)
		{
			hasSwsCtx = false;
			sws_freeContext(pSwsCtx);
			pSwsCtx = sws_alloc_context();
			return false;
		}
		if (got_picture)
		{
			if (!hasSwsCtx)
			{
				hasSwsCtx = true;
				pSwsCtx = sws_getCachedContext(pSwsCtx, pCodecCtx->width,
					pCodecCtx->height, pCodecCtx->pix_fmt,
					pCodecCtx->width, pCodecCtx->height,
					AV_PIX_FMT_YUV420P, SWS_BILINEAR, 0, 0, 0);
				ret = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
				pFrameOutBuf = (uint8_t*)av_malloc(ret);
				av_image_fill_arrays(pFrameOut->data, pFrameOut->linesize,
					pFrameOutBuf, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
			}
			sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0,
				pFrame->height, pFrameOut->data, pFrameOut->linesize);
			*out_frame = pFrameOut;
			av_packet_unref(&packet);
			return true;
		}
	}
}

bool CH264Decoder::internal_Init()
{
	if (!isInitializedH264Decoder)
	{
		pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
		if (!pCodec)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not find the H264 decoder.\n");
			return false;
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not allocate codec context for H264 decoder.\n");
			return false;
		}
		pH264DecoderParserCtx = av_parser_init(AV_CODEC_ID_H264);
		if (!pH264DecoderParserCtx)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not initialize codec parser context for H264 decoder.\n");
			return false;
		}
		pSwsCtx = sws_alloc_context();
		if (!pSwsCtx)
		{
			av_log(0, AV_LOG_ERROR, "CH264Decoder:Could not allocate swscontext for H264 decoder.\n");
			return false;
		}
		isInitializedH264Decoder = true;
		hasSwsCtx = false;
		yLen = uvLen = 0;
		cur_size = 0;
		memset(&packet, 0, sizeof(AVPacket));
		pFrame = pFrameOut = NULL;
		pFrameOutBuf = NULL;
		av_log(0, AV_LOG_INFO, "CH264Decoder has been initialized.\n");
	}
	return true;
}

AVFrame *CH264Decoder::copy_yuvFrame(const AVFrame *yuvPic)
{
	AVFrame *out;
	out = av_frame_alloc();
	out->width = pCodecCtx->width;
	out->height = pCodecCtx->height;
	out->format = pCodecCtx->pix_fmt;
	av_image_alloc(out->data, out->linesize, pCodecCtx->width, pCodecCtx->height,
		AV_PIX_FMT_YUV420P, 16);
	av_image_copy(out->data, out->linesize, 
		(const uint8_t**)yuvPic->data, yuvPic->linesize, 
		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
//	av_image_fill_arrays(out->data, out->linesize,
//		yuvPic->data[0], pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);

	return out;
}

void CH264Decoder::free_yuvFrame(AVFrame **yuvPic)
{
	av_freep(yuvPic[0]->data);
	av_frame_free(yuvPic);
}