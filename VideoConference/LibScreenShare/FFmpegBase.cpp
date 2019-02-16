#include "FFmpegBase.h"
bool CFFmpegCodecBase::isInitializedFFmpeg = false;
CFFmpegCodecBase::CFFmpegCodecBase()
{
	if (!isInitializedFFmpeg)
	{
		av_register_all();
		av_log(0, AV_LOG_INFO, "FFmpeg has been initialized.\n");
		isInitializedFFmpeg = true;
	}
	pCodecCtx = NULL;
	pCodec = NULL;
}

bool CFFmpegCodecBase::getYUV420PLength(size_t *YLen, size_t *UVLen) const
{
	int y_size = pCodecCtx->width * pCodecCtx->height;
	if (!y_size)
		return false;
	if (YLen)
		*YLen = y_size;
	if (UVLen)
		*UVLen = y_size >> 2;
	return true;
}

bool CFFmpegCodecBase::getSize(int &w, int &h) const
{
	if (!pCodecCtx->width)
		return false;
	w = pCodecCtx->width;
	h = pCodecCtx->height;
	return true;
}
int CFFmpegCodecBase::getFPS() const
{
	if (!pCodecCtx->width)
		return 0;
	else
		return pCodecCtx->framerate.num;
	return true;
}
