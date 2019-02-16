#include "MP3Codec.h"
CMP3Encoder::CMP3Encoder()
{
	isInitializedMP3Encoder = false;
	internal_Init();
}
CMP3Encoder::~CMP3Encoder()
{
	avcodec_free_context(&pCodecCtx);
	swr_free(&pSwrCtx);
	if (pFrameDataBuf)
		av_freep(&pFrameDataBuf);
	if (pFrame)
		av_frame_free(&pFrame);
}
CMP3Encoder::CMP3Encoder(uint64_t channel_layout, 
	int bit_rate, int sample_rate)
{
	isInitializedMP3Encoder = false;
	internal_Init();
	setupEncoder(channel_layout, bit_rate, sample_rate);
}
bool CMP3Encoder::setupEncoder(uint64_t channel_layout, int bit_rate, int sample_rate)
{
	int ret;
	pCodecCtx->bit_rate = bit_rate;
	pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16P;
	pCodecCtx->sample_rate = sample_rate;
	pCodecCtx->channel_layout = channel_layout;
	pCodecCtx->channels = av_get_channel_layout_nb_channels(channel_layout);
	
	pFrame = av_frame_alloc();
	if (!pFrame)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not allocate audio frame for MP3 encoder..\n");
		return false;
	}
	av_init_packet(&packet);
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not open CMP3Encoder.\n");
		return false;
	}
	
	pSwrCtx = swr_alloc_set_opts(0, pCodecCtx->channel_layout, pCodecCtx->sample_fmt,
		pCodecCtx->sample_rate, pCodecCtx->channel_layout, 
		AV_SAMPLE_FMT_S16, pCodecCtx->sample_rate, 0, 0);
	
	if (!pSwrCtx)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not allocate resampler for MP3 Encoder.\n");
		return false;
	}

	ret = swr_init(pSwrCtx);
	if (ret < 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not initialize resampler for MP3 Encoder.\n");
		return false;
	}

	pFrame->nb_samples = pCodecCtx->frame_size;
	pFrame->format = pCodecCtx->sample_fmt;
	pFrame->channel_layout = pCodecCtx->channel_layout;

	bufferSize = av_samples_get_buffer_size(0, pCodecCtx->channels,
		pCodecCtx->frame_size, pCodecCtx->sample_fmt, 0);

	if (bufferSize < 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not get sample buffer size\n");
		return false;
	}

	pFrameDataBuf = (uint16_t*)av_malloc(bufferSize);
	if (!(pFrame && pFrameDataBuf))
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not allocate memory to frame.\n",
			pFrameDataBuf);
		return false;
	}
	ret = avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,
		(const uint8_t*)pFrameDataBuf, bufferSize, 0);
	
	if (ret < 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not setup audio frame.\n");
		return false;
	}
	av_log(0, AV_LOG_INFO, "CMP3Encoder codec has been opened, and being able to work.\n");
	return true;
}
bool CMP3Encoder::encodeFile(const char *inPCM_2CH_2BYTES_File, const char *outMP3File)
{
	int ret, got_output;
	uint32_t size(0);
	uint8_t *in_buffer;
	std::ifstream in_pcm(inPCM_2CH_2BYTES_File, std::ios::binary | std::ios::in);
	std::ofstream out_aac(outMP3File, std::ios::binary | std::ios::out);
	if (!in_pcm.is_open())
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not open the file from [%s]\n", inPCM_2CH_2BYTES_File);
		return false;
	}
	if (!out_aac.is_open())
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not write the file to [%s]\n", outMP3File);
		in_pcm.close();
		return false;
	}
	av_log(0, AV_LOG_INFO, "CMP3Encoder:Input file path:[%s]\n", inPCM_2CH_2BYTES_File);
	av_log(0, AV_LOG_INFO, "CMP3Encoder:Output file path:[%s]\n", outMP3File);
	av_log(0, AV_LOG_INFO, "CMP3Encoder:Start encoding...");
	in_buffer = (uint8_t*)av_malloc(bufferSize);
	packet.data = NULL;
	packet.size = 0;
	while (!in_pcm.eof())
	{
		in_pcm.read((char*)in_buffer, bufferSize);
		pFrame->nb_samples = (int)in_pcm.gcount() >> 2; //2ch 2bytes		
		swr_convert(pSwrCtx, pFrame->data, pFrame->nb_samples,
			(const uint8_t**)&in_buffer, pFrame->nb_samples);

		ret = avcodec_encode_audio2(pCodecCtx, &packet, pFrame, &got_output);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Encoder:Error encoding audio frame\n");
			return false;
		}
		if (got_output)
		{
			out_aac.write((char*)packet.data, packet.size);
			av_packet_unref(&packet);
		}
	}
	for (got_output = 1; got_output;)
	{
		ret = avcodec_encode_audio2(pCodecCtx, &packet, NULL, &got_output);
		if (ret < 0)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Encoder:Error encoding frame\n");
			return false;
		}
		if (got_output)
		{
			out_aac.write((char*)packet.data, packet.size);
			av_packet_unref(&packet);
		}
	}
	pFrame->nb_samples = pCodecCtx->frame_size;
	av_log(0, AV_LOG_INFO, "\nCMP3Encoder:Encoding has been done.\n");
	av_free(in_buffer);
	in_pcm.close();
	out_aac.close();
	return true;
}
int CMP3Encoder::encodeFragment(uint8_t *rawPCM_2CH_2BYTES, size_t raw_length, uint8_t **out_MP3RawData)
{
	int got_output;
	static bool hasOut;
	pFrame->data[0] = rawPCM_2CH_2BYTES;
	pFrame->nb_samples = raw_length >> 2;
	if (hasOut)
	{
		hasOut = false;
		av_packet_unref(&packet);
	}
	swr_convert(pSwrCtx, pFrame->data, pFrame->nb_samples,
		(const uint8_t**)&rawPCM_2CH_2BYTES, pFrame->nb_samples);
	if (avcodec_encode_audio2(pCodecCtx, &packet, pFrame, &got_output) >= 0)
	{
		if (got_output)
		{
			hasOut = true;
			*out_MP3RawData = packet.data;
			return packet.size;
		}
		return 0;
	}
	else
		return -1;
}
int CMP3Encoder::flushFrameOut(uint8_t **out_MP3RawData)
{
	int got_output, ret;
	static bool hasOut;
	if (hasOut)
	{
		hasOut = false;
		av_packet_unref(&packet);
	}
	ret = avcodec_encode_audio2(pCodecCtx, &packet, NULL, &got_output);
	if (ret < 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Encoder:Error encoding frame\n");
		return false;
	}
	if (got_output)
	{
		hasOut = true;
		*out_MP3RawData = packet.data;
		return packet.size;
	}
	else
	{
		pFrame->data[0] = (uint8_t*)pFrameDataBuf;
		return false;
	}
}
void CMP3Encoder::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----MP3 Encoder Information----\n");
	av_log(0, AV_LOG_INFO, "\tBitrate:%d\n", pCodecCtx->bit_rate);
	av_log(0, AV_LOG_INFO, "\tChannels:%d\n", pCodecCtx->channels);
	av_log(0, AV_LOG_INFO, "\tSample rate:%d\n", pCodecCtx->sample_rate);
	av_log(0, AV_LOG_INFO, "\tFormat:Signed 16-bit PCM\n");
}
bool CMP3Encoder::internal_Init()
{
	if (!isInitializedMP3Encoder)
	{
		pCodec = avcodec_find_encoder(AV_CODEC_ID_MP3);
		if (!pCodec)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not find the MP3 encoder.\n");
			return false;
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Encoder:Could not allocate codec context for MP3 encoder.\n");
			return false;
		}
		isInitializedMP3Encoder = true;
		pSwrCtx = NULL;
		pFrame = NULL;
		bufferSize = 0;
		memset(&packet, 0, sizeof(packet));
		av_log(0, AV_LOG_INFO, "CMP3Encoder has been initialized.\n");
	}
	return true;
}
size_t CMP3Encoder::getBufferSize() const
{
	return bufferSize;
}
CMP3Decoder::CMP3Decoder()
{
	isInitializedMP3Decoder = false;
	internal_Init();
}
CMP3Decoder::~CMP3Decoder()
{
	avcodec_free_context(&pCodecCtx);
	av_parser_close(pCodecParserCtx);
	if (hasSwrCtx)
		swr_free(&pSwrCtx);
	if (pFrame)
		av_frame_free(&pFrame);
	if (out_buf)
		av_freep(&out_buf);
}
CMP3Decoder::CMP3Decoder(int out_channels, int out_sample_rate)
{
	isInitializedMP3Decoder = false;
	internal_Init();
	setupDecoder(out_channels, out_sample_rate);
}
void CMP3Decoder::setupDecoder(int out_channels, int out_sample_rate)
{
	this->out_channels = out_channels;
	this->out_sample_rate = out_sample_rate;
	av_init_packet(&packet);
	pFrame = av_frame_alloc();
	if (avcodec_open2(pCodecCtx, pCodec, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CMP3Decoder:Could not open AAC decoder context.");
	}
	av_log(0, AV_LOG_INFO, "CMP3Decoder codec has been opened, and being able to work.\n");
}
void CMP3Decoder::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----MP3 Decoder Information----\n");
	av_log(0, AV_LOG_INFO, "\tBitrate:%d\n", pCodecCtx->bit_rate);
	av_log(0, AV_LOG_INFO, "\tChannels:%d\n", pCodecCtx->channels);
	av_log(0, AV_LOG_INFO, "\tSample rate:%d\n", pCodecCtx->sample_rate);
	av_log(0, AV_LOG_INFO, "\tFormat:MP3\n");
	av_log(0, AV_LOG_INFO, "----MP3 Decoder Output Info----\n");
	av_log(0, AV_LOG_INFO, "\tChannels:%d\n", out_channels);
	av_log(0, AV_LOG_INFO, "\tSample rate:%d\n", out_sample_rate);
	av_log(0, AV_LOG_INFO, "\tFormat:Signed 16-bit PCM\n");
}
int CMP3Decoder::decodeFile(const char *inMP3File, const char *outPCMFile)
{
	int error_cnt = 0;
	std::ofstream outFile(outPCMFile, std::ios::binary | std::ios::out);
	AVFormatContext *pFormatCtx(NULL);
	int got_frame, ret, out_buffer_size;
	if (avformat_open_input(&pFormatCtx, inMP3File, 0, 0) != 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not open the file from [%s]\n", inMP3File);
		return 0;
	}
	if (avformat_find_stream_info(pFormatCtx, 0) < 0)
	{
		av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not find the stream info from [%s]\n", inMP3File);
		return 0;
	}
	av_dump_format(pFormatCtx, 0, inMP3File, 0);
	if (!outFile.is_open())
	{
		av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not write the file to [%s]\n", outPCMFile);
		return 0;
	}
	av_log(0, AV_LOG_INFO, "CMP3Decoder:Input file path:[%s]\n", inMP3File);
	av_log(0, AV_LOG_INFO, "CMP3Decoder:Output file path:[%s]\n", outPCMFile);
	av_log(0, AV_LOG_INFO, "CMP3Decoder:Start decoding...");
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		ret = avcodec_decode_audio4(pCodecCtx, pFrame, &got_frame, &packet);
		if (ret < 0)
		{
			error_cnt++;
			if (error_cnt > 5)
			{
				av_log(0,AV_LOG_FATAL,"CMP3Decoder:Too many errors, decoding has been stopped!\n");
				return 0;
			}
			continue;
		}
		if (got_frame)
		{
			if (!hasSwrCtx)
			{
				hasSwrCtx = true;
				pSwrCtx = swr_alloc_set_opts(pSwrCtx,
					av_get_default_channel_layout(out_channels),
					AV_SAMPLE_FMT_S16, out_sample_rate, pCodecCtx->channel_layout,
					pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, 0);
				out_buf = (uint8_t*)av_malloc(MAX_OUT_BUF_SIZE * 2);
				swr_init(pSwrCtx);
			}
			ret = swr_convert(pSwrCtx, &out_buf, MAX_OUT_BUF_SIZE,
				(const uint8_t**)pFrame->data, pFrame->nb_samples);
			out_buffer_size = av_samples_get_buffer_size(0, out_channels,
				ret, AV_SAMPLE_FMT_S16, 1);
			outFile.write((char*)out_buf, out_buffer_size);
			av_packet_unref(&packet);
		}
	}
	av_log(0, AV_LOG_INFO, "\nCMP3Decoder:Decoding has been done.\n");
	decodeMP3Close();
	avformat_close_input(&pFormatCtx);
	outFile.close();
	return 1;
}
int CMP3Decoder::decodeFile(const char *inMP3File, uint8_t **out_PCMData, int &out_PCMDataLength)
{
	int error_cnt = 0;
	static AVFormatContext *pFormatCtx;
	int got_frame, ret;
	if (!pFormatCtx)
	{
		if (avformat_open_input(&pFormatCtx, inMP3File, 0, 0) != 0)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not open the file from [%s]\n", inMP3File);
			return -1;
		}
		if (avformat_find_stream_info(pFormatCtx, 0) < 0)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not find the stream info from [%s]\n", inMP3File);
			return -1;
		}
		av_dump_format(pFormatCtx, 0, inMP3File, 0);
	}
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		ret = avcodec_decode_audio4(pCodecCtx, pFrame, &got_frame, &packet);
		if (ret < 0)
		{
			error_cnt++;
			if (error_cnt > 5)
			{
				av_log(0, AV_LOG_FATAL, "CMP3Decoder:Too many errors, decoding has been stopped!\n");
				return 0;
			}
			continue;
		}
		if (got_frame)
		{
			if (!hasSwrCtx)
			{
				hasSwrCtx = true;
				pSwrCtx = swr_alloc_set_opts(pSwrCtx,
					av_get_default_channel_layout(out_channels),
					AV_SAMPLE_FMT_S16, out_sample_rate, pCodecCtx->channel_layout,
					pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, 0);
				out_buf = (uint8_t*)av_malloc(MAX_OUT_BUF_SIZE * 2);
				swr_init(pSwrCtx);
			}
			ret = swr_convert(pSwrCtx, &out_buf, MAX_OUT_BUF_SIZE,
				(const uint8_t**)pFrame->data, pFrame->nb_samples);
			*out_PCMData = out_buf;
			out_PCMDataLength = av_samples_get_buffer_size(0, out_channels,
				ret, AV_SAMPLE_FMT_S16, 1);

			av_packet_unref(&packet);
			return 1;
		}
	}
	decodeMP3Close();
	avformat_close_input(&pFormatCtx);
	return 0;
}
int CMP3Decoder::decodeMP3(uint8_t *rawAACdata, int dataLength, uint8_t **out_PCMData, int &out_PCMDataLength)
{
	int got_frame, ret;
	static uint8_t *cur_ptr;
	static int error_cnt;
	if (!cur_size)
	{
		cur_ptr = rawAACdata;
		cur_size = dataLength;
	}
	while (cur_size > 0)
	{
		int len = av_parser_parse2(
			pCodecParserCtx, pCodecCtx,
			&packet.data, &packet.size,
			cur_ptr, cur_size,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		cur_ptr += len;
		cur_size -= len;
		if (packet.size == 0)
			continue;
		ret = avcodec_decode_audio4(pCodecCtx, pFrame, &got_frame, &packet);
		if (got_frame)
		{
			if (!hasSwrCtx)
			{
				hasSwrCtx = true;
				pSwrCtx = swr_alloc_set_opts(pSwrCtx,
					av_get_default_channel_layout(out_channels),
					AV_SAMPLE_FMT_S16, out_sample_rate,
					pCodecCtx->channel_layout, pCodecCtx->sample_fmt,
					pCodecCtx->sample_rate, 0, 0);
				out_buf = (uint8_t*)av_malloc(MAX_OUT_BUF_SIZE * 2);
				swr_init(pSwrCtx);
			}
			ret = swr_convert(pSwrCtx, &out_buf,
				MAX_OUT_BUF_SIZE,
				(const uint8_t**)pFrame->data, pFrame->nb_samples);
			if (ret < 0)
			{
				av_log(0, AV_LOG_FATAL, "CMP3Decoder:Could not re-sample!");
				return -1;
			}
			out_PCMDataLength = av_samples_get_buffer_size(0, out_channels,
				ret, AV_SAMPLE_FMT_S16, 1);
			*out_PCMData = out_buf;
			av_packet_unref(&packet);
			if (cur_size)
				return 2;
			else
				return 1;
		}
	}
	return 0;
}
void CMP3Decoder::decodeMP3Close()
{
	hasSwrCtx = false;
	if (hasSwrCtx)
		swr_free(&pSwrCtx);
	pSwrCtx = swr_alloc();
	cur_size = 0;
}
void CMP3Decoder::decodeMP3Reset()
{
	decodeMP3Close();
}
bool CMP3Decoder::internal_Init()
{
	if (!isInitializedMP3Decoder)
	{
		pCodec = avcodec_find_decoder(AV_CODEC_ID_MP3);
		if (!pCodec)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not find the AAC decoder.\n");
			return false;
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not allocate codec context for AAC decoder.\n");
			return false;
		}
		pCodecParserCtx = av_parser_init(pCodec->id);
		if (!pCodecParserCtx)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not initialize codec parser context for AAC decoder.\n");
			return false;
		}
		pSwrCtx = swr_alloc();
		if (!pSwrCtx)
		{
			av_log(0, AV_LOG_ERROR, "CMP3Decoder:Could not allocate re-sampler context for AAC decoder.\n");
			return false;
		}
		isInitializedMP3Decoder = true;
		memset(&packet, 0, sizeof(AVPacket));
		pFrame = nullptr;
		hasSwrCtx = false;
		out_buf = nullptr;
		cur_size = out_channels = out_sample_rate = 0;
		av_log(0, AV_LOG_INFO, "CMP3Decoder has been initialized.\n");
	}
	return true;
}