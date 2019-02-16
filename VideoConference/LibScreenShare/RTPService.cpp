#include "RTPService.h"
CRTPSender::CRTPSender()
{
	isInitializedRTPSender = false;
	internal_Init();
}
CRTPSender::~CRTPSender()
{
	//finish();
}
CRTPSender::CRTPSender(const char *destIP, const int destPort, TRANSMISSION_TYPE type,
	float FPS, int audio_sample_rate, int video_width, int video_height)
{
	isInitializedRTPSender = false;
	internal_Init();
	setupRTPSender(destIP, destPort, type, FPS, audio_sample_rate, video_width, video_height);
}
bool CRTPSender::setupRTPSender(const char *destIP, const int destPort, TRANSMISSION_TYPE type,
	float FPS, int audio_sample_rate, int video_width, int video_height)
{
	this->destIP.assign(destIP);
	this->destPort = destPort;
	this->frame_delay_ms = (int)((1.0 / FPS) * 1000);
	this->trans_type = type;
	this->width = video_width;
	this->height = video_height;
	this->sample_rate = audio_sample_rate;

	avformat_network_init();
	pRtpFormat = avformat_alloc_context();
	pRtpFormat->oformat = av_guess_format("rtp", 0, 0);
	sprintf(pRtpFormat->filename, "rtp://%s:%d", destIP, destPort);
	if (avio_open(&pRtpFormat->pb, pRtpFormat->filename,
		AVIO_FLAG_WRITE) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CRTPSender:Could not initialize I/O open for AVFormatContext.\n");
		return false;
	}

	stream = avformat_new_stream(pRtpFormat, 0);
	pCodecCtx = stream->codec;
	if (FPS < 1)
	{
		stream->time_base.den = 1;
		stream->time_base.num = (int)(1.0 / FPS);
	}
	else
	{
		stream->time_base.den = (int)FPS;
		stream->time_base.num = 1;
	}
	pCodecCtx->time_base.den = (int)FPS;
	pCodecCtx->time_base.num = 1;

	if (type == MEDIA_H264_VIDEO)
	{
		pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
		pCodecParserCtx = av_parser_init(AV_CODEC_ID_H264);
		avcodec_get_context_defaults3(pCodecCtx, pCodec);
		pCodecCtx->width = width;
		pCodecCtx->height = height;
		pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	}
	else if (type == MEDIA_MP3_AUDIO)
	{
		pCodec = avcodec_find_encoder(AV_CODEC_ID_MP3);
		pCodecParserCtx = av_parser_init(AV_CODEC_ID_MP3);
		avcodec_get_context_defaults3(pCodecCtx, pCodec);
		pCodecCtx->sample_rate = audio_sample_rate;
		pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16P;
	}
	avformat_write_header(pRtpFormat, NULL);
	if (av_sdp_create(&pRtpFormat, 1, sdp_context,
		sizeof(sdp_context)) < 0)
	{
		av_log(0, AV_LOG_INFO, "CRTPSender:Creating SDP file failed!");
		return false;
	}
	av_log(pRtpFormat, AV_LOG_INFO, "CRTPSender:Sender is ready to send. \n SDP context \n%s\n",
		sdp_context);
	return true;
}
void CRTPSender::start()
{
	if (!t1)
	{
		pkt_que.reset();
		t1 = new std::thread(&CRTPSender::transmission_handle, this);
	}
	else
		av_log(0, AV_LOG_FATAL, "CRTPSender: You can't start sender twice!\n");
}
void CRTPSender::finish()
{
	finished = true;
	pkt_que.break_getItem();
	if (t1)
	{
		t1->join();
		delete t1;
		t1 = nullptr;
	}
	while (auto k = pkt_que.flushOutItem())
	{
		av_free_packet(&k->data);
		delete k;
	}
	if (pRtpFormat)
		avformat_close_input(&pRtpFormat);
	if (pCodecParserCtx)
		av_parser_close(pCodecParserCtx);
	av_packet_unref(&packet);
	pRtpFormat = nullptr;
	pCodecParserCtx = nullptr;
}

bool CRTPSender::writeSDPFile(const char *filename) const
{
	std::ofstream file(filename, std::ios::binary);
	if (!file)
	{
		av_log(0, AV_LOG_FATAL, "CRTPSender:Could not output file at [%s]\n", filename);
		return false;
	}
	file.write(sdp_context, strlen(sdp_context));
	file.close();
	return true;
}

void CRTPSender::appendRawData(uint8_t *raw_data, int raw_length)
{
	AVPacket pkt;
	int cur_size, len;
	uint8_t *cur_ptr;
	cur_ptr = raw_data;
	cur_size = raw_length;

	while (cur_size > 0)
	{
		len = av_parser_parse2(
			pCodecParserCtx, pCodecCtx,
			&packet.data, &packet.size,
			cur_ptr, cur_size,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		cur_ptr += len;
		cur_size -= len;
		if (packet.size == 0)
			continue;

		packet.pts = packet.dts = frame_serial_num++;


		
		
		av_copy_packet(&pkt, &packet);
		av_free_packet(&packet);

		pkt_que.push(pkt);
		
	}
}
int CRTPSender::getItemCount()
{
	return pkt_que.getSize();
}
const char *CRTPSender::getSDPcontext() const
{
	return sdp_context;
}
void CRTPSender::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----RTP Sender Information----\n");
	av_log(0, AV_LOG_INFO, "\tBitrate:%d\n", pCodecCtx->bit_rate);
	av_log(0, AV_LOG_INFO, "\tDestination IP:%s\ns", destIP);
	av_log(0, AV_LOG_INFO, "\tDestination Port:%d\ns", destPort);
	av_log(0, AV_LOG_INFO, "\tFPS:%d\n", (int)(1.0 / frame_delay_ms * 1000));
	if (trans_type == MEDIA_H264_VIDEO)
	{
		av_log(0, AV_LOG_INFO, "\tWidth:%d\n", pCodecCtx->width);
		av_log(0, AV_LOG_INFO, "\tHeight:%d\n", pCodecCtx->height);
		av_log(0, AV_LOG_INFO, "\tMode:%s\n", "H264 VIDEO");
	}
	else if (trans_type == MEDIA_MP3_AUDIO)
	{
		av_log(0, AV_LOG_INFO, "\tMode:%s\n", "MP3 AUDIO");
	}
}
bool CRTPSender::internal_Init()
{
	if (!isInitializedRTPSender)
	{
		t1 = NULL;
		destIP.clear();
		destPort = 0;
		frame_delay_ms = 0;
		width = 0;
		height = 0;
		trans_type = MEDIA_NONE;
		pkt_que.setMaxSize(30);
		frame_serial_num = 0;
		memset(sdp_context, 0, sizeof(sdp_context));
		isInitializedRTPSender = true;
		return true;
	}
	return false;
}
void CRTPSender::transmission_handle()
{
	CPacketList<AVPacket> *item;
	AVPacket pkt;
	finished = false;
	while (!finished)
	{
		item = pkt_que.getItem();
		if (!item)
			break;
		pkt = item->data;
		if (pkt.pts != AV_NOPTS_VALUE)
		{
			pkt.pts = av_rescale_q(pkt.pts, stream->codec->time_base, stream->time_base);
		}

		if (pkt.dts != AV_NOPTS_VALUE)
		{
			pkt.dts = av_rescale_q(pkt.dts, stream->codec->time_base, stream->time_base);
		}
		av_write_frame(pRtpFormat, &pkt);
		if (this->trans_type == MEDIA_H264_VIDEO)
			Sleep(frame_delay_ms);

		av_free_packet(&pkt);
		delete item;
	}
}
//Receiver
CRTPReceiver::CRTPReceiver()
{
	isInitializedRTPReceiver = false;
	internal_Init();
}
CRTPReceiver::~CRTPReceiver()
{
	finish();
}
CRTPReceiver::CRTPReceiver(const char *sdp_filename, int receiver_buffer_size)
{
	isInitializedRTPReceiver = false;
	internal_Init();
	setupRTPReceiver(sdp_filename, receiver_buffer_size);
}
bool CRTPReceiver::setupRTPReceiver(const char *sdp_filename, int receiver_buffer_size)
{
	memcpy(this->sdp_filename, sdp_filename, strlen(sdp_filename));
	memset(&time_out, 0, sizeof(time_out));
	time_out.first_play = true;
	pRtpFormat = avformat_alloc_context();
	pRtpFormat->interrupt_callback.callback = interrupt_cb;
	pRtpFormat->interrupt_callback.opaque = &time_out;
	pkt_que.setMaxSize(receiver_buffer_size);
	if (avformat_open_input(&pRtpFormat, sdp_filename, 0, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CRTPReceiver: Could not open input.\n");
		failure = true;
		return false;
	}
	av_log(0, AV_LOG_INFO, "RTPReceiver has been setup.\n");
	return true;
}
void CRTPReceiver::start()
{
	if (!t1)
		t1 = new std::thread(&CRTPReceiver::transmission_handle, this);
	else
	{
		av_log(0, AV_LOG_FATAL, "CRTPReceiver: You can't start receiver twice!\n");
	}
}
void CRTPReceiver::pause()
{
	isStopped = true;
}
void CRTPReceiver::restart()
{
	isStopped = false;
}
void CRTPReceiver::finish()
{
	finished = true;
	time_out.forceStop = true;
	if (t1)
	{
		pkt_que.break_getItem();
		t1->join();
		t1 = NULL;
	}
	while (auto k = pkt_que.flushOutItem())
	{
		av_free_packet(&k->data);
		delete k;
	}
	if (pCodecCtx)
	{
		avcodec_free_context(&pCodecCtx);
		pCodecCtx = NULL;
	}
	if (pRtpFormat)
	{
		avformat_free_context(pRtpFormat);
		pRtpFormat = NULL;
	}
	pkt_que.reset();
}
RawItem CRTPReceiver::getItem(int itemlessthan)
{
	RawItem out = { 0 };
	out.type = getMediaType();
	if (item)
	{
		av_free((item->data).data);
		av_free_packet(&item->data);
		delete item;
	}
	if (!finished)
	{
		item = pkt_que.getItemUntil(itemlessthan);
		//item = pkt_que.getItem();
		if (item)
		{
			out.raw_data = (item->data).data;
			out.raw_length = (item->data).size;
		}
	}
	else
	{
		item = pkt_que.flushOutItem();
		if (item)
		{
			out.raw_data = (item->data).data;
			out.raw_length = (item->data).size;
		}
	}
	return out;
}
int CRTPReceiver::getItemCount()
{
	return pkt_que.getSize();
}
TRANSMISSION_TYPE CRTPReceiver::getMediaType()
{
	switch (pRtpFormat->streams[0]->codec->codec_id)
	{
	case AV_CODEC_ID_H264:
		return MEDIA_H264_VIDEO;
	case AV_CODEC_ID_MP3:
		return MEDIA_MP3_AUDIO;
	default:
		return MEDIA_NONE;
	}
}
bool CRTPReceiver::isNoError()
{
	return !failure;
}
void CRTPReceiver::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----RTP Sender Information----\n");
	av_log(0, AV_LOG_INFO, "\tBitrate:%d\n", pCodecCtx->bit_rate);
	if (pRtpFormat->streams[0]->codec->codec_id == AV_CODEC_ID_H264)
	{
		av_log(0, AV_LOG_INFO, "\tWidth:%d\n", pCodecCtx->width);
		av_log(0, AV_LOG_INFO, "\tHeight:%d\n", pCodecCtx->height);
		av_log(0, AV_LOG_INFO, "\tMode:%s\n", "H264 VIDEO");
	}
	else if (pRtpFormat->streams[0]->codec->codec_id == AV_CODEC_ID_MP3)
	{
		av_log(0, AV_LOG_INFO, "\tMode:%s\n", "MP3 AUDIO");
	}
}
void CRTPReceiver::transmission_handle()
{
	AVPacket pkt;
	finished = false;
	isStopped = false;
	if (avformat_find_stream_info(pRtpFormat, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CRTPReceiver: Could not find stream info.\n");
		pkt_que.break_getItem();
		finished = true;
		failure = true;
		return;
	}
	time_out.first_play = false;
	while (av_read_frame(pRtpFormat, &packet) >= 0)
	{
		if (!isStopped)
		{
			if (finished)
				break;
			av_copy_packet(&pkt, &packet);
			pkt_que.push(pkt);
		}
		else
			av_packet_unref(&packet);
	}
	av_log(0, AV_LOG_INFO, "CRTPReceiver: Receiving has done.\n");
	pkt_que.break_getItem();
	finished = true;
}
int interrupt_cb(void *ctx)
{
	TimeOut *time_out = (TimeOut*)ctx;
	if (time_out->first_play)
	{
		if (!time_out->s)
			time_out->s = av_gettime_relative() / AV_TIME_BASE;

		time_out->drift = av_gettime_relative() / AV_TIME_BASE - time_out->s;

		if (time_out->drift > 100)
		{
			av_log(0, AV_LOG_INFO, "Time out!\n");
			return 1;
		}
	}
	return time_out->forceStop;
}
bool CRTPReceiver::internal_Init()
{
	if (!isInitializedRTPReceiver)
	{
		avformat_network_init();
		isInitializedRTPReceiver = true;
		memset(&time_out, 0, sizeof(time_out));
		pRtpFormat = NULL;
		stream = NULL;
		item = NULL;
		t1 = NULL;
		isStopped = false;
		finished = false;
		failure = false;
		av_init_packet(&packet);
		return true;
	}
	return false;
}
