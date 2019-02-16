#include "ImageUtils.h"
CImageFormatConverter::CImageFormatConverter()
{
	isInitializedIMGCvt = false;
	internal_Init();
}
CImageFormatConverter::CImageFormatConverter(int srcW, int srcH,
	int destW, int destH, AVPixelFormat inPIX_FMT,
	AVPixelFormat outPIX_FMT)
{
	isInitializedIMGCvt = false;
	internal_Init();
	setupConverter(srcW, srcH, destW, destH, inPIX_FMT, outPIX_FMT);
}
bool CImageFormatConverter::setupConverter(int srcW, int srcH,
	int destW, int destH, AVPixelFormat inPIX_FMT, AVPixelFormat outPIX_FMT)
{

	inPixelFormat = inPIX_FMT;
	outPixelFormat = outPIX_FMT;
	srcWidth = srcW;
	srcHeight = srcH;
	destWidth = destW;
	destHeight = destH;
	pSwsCtx = sws_getCachedContext(pSwsCtx, srcWidth, srcHeight, inPixelFormat,
		destW, destH, outPixelFormat, SWS_BILINEAR, 0, 0, 0);
	if (!pSwsCtx)
	{
		av_log(0, AV_LOG_ERROR, "CImageFormatConverter:Could not allocate image converter for CImageFormatConverter.\n");
		return false;
	}
	outPicture = makePicture(destW, destH, outPixelFormat);
	if (!outPicture)
	{
		av_log(0, AV_LOG_ERROR, "CImageFormatConverter:Could not allocate image for CImageFormatConverter.\n");
		return false;
	}
	AVFrame *t;
	t = makePicture(srcW, srcH, AV_PIX_FMT_BGR24);
	inv_inlineSize[0] = -t->linesize[0];
	inv_srcSlice_val[0] = t->linesize[0] * (t->height - 1);
	freePicture(&t);

	av_log(0, AV_LOG_INFO, "CImageFormatConverter is up to work.\n");
	return true;
}
void CImageFormatConverter::convert(uint8_t *srcData, AVFrame **out)
{
	AVFrame *inFrame = makePicture(srcWidth, srcHeight, inPixelFormat, srcData);
	sws_scale(pSwsCtx, inFrame->data, inFrame->linesize, 0, inFrame->height,
		outPicture->data, outPicture->linesize);
	*out = outPicture;
	av_frame_free(&inFrame);
}
void CImageFormatConverter::inv_convert(uint8_t *srcData, AVFrame **out)
{
	AVFrame *inFrame = makePicture(srcWidth, srcHeight, inPixelFormat, srcData);
	uint8_t *srcSlice[1] =
	{ inFrame->data[0] + inv_srcSlice_val[0] };
	int inlineSize[1] =
	{ -inFrame->linesize[0] };
	sws_scale(pSwsCtx, srcSlice, inlineSize, 0, inFrame->height,
		outPicture->data, outPicture->linesize);
	*out = outPicture;
	av_frame_free(&inFrame);
}
AVFrame* CImageFormatConverter::makePicture(int w, int h, AVPixelFormat pixelFmt, uint8_t *data)
{
	int align = (pixelFmt == AV_PIX_FMT_BGR24) || (pixelFmt == AV_PIX_FMT_RGB24) ? 4 : 1;
	AVFrame *pFrame = NULL;
	pFrame = av_frame_alloc();
	pFrame->width = w;
	pFrame->height = h;
	pFrame->format = pixelFmt;
	if (!data)
	{
		av_image_alloc(pFrame->data, pFrame->linesize, w, h, pixelFmt, align);
	}
	else
	{
		av_image_fill_arrays(pFrame->data, pFrame->linesize,
			data, pixelFmt, w, h, align);
	}
	return pFrame;
}
int CImageFormatConverter::getPictureSize(int w, int h, AVPixelFormat pixelFmt)
{
	int align = pixelFmt == AV_PIX_FMT_BGR24 ? 4 : 1;
	return av_image_get_buffer_size(pixelFmt, w, h, align);
}
void CImageFormatConverter::freePicture(AVFrame **pic)
{
	if (pic)
	{
		av_freep((*pic)->data);
		av_frame_free(pic);
	}
}
void CImageFormatConverter::showProperties() const
{

}
bool CImageFormatConverter::internal_Init()
{
	if (!isInitializedIMGCvt)
	{
		srcWidth = srcHeight = destWidth = destHeight = 0;
		pSwsCtx = sws_alloc_context();
		inPixelFormat = AV_PIX_FMT_NONE;
		outPixelFormat = AV_PIX_FMT_NONE;
		isInitializedIMGCvt = true;
		outPicture = NULL;
		pFrameOutBuf = NULL;
		av_log(0, AV_LOG_INFO, "CImageUtils has been initialized.\n");
	}
	return true;
}
CImageIO::CImageIO()
{
	internal_Init();
}
CImageIO::~CImageIO()
{
	if (raw_data)
	{
		delete[] raw_data;
	}
}
CImageIO::CImageIO(const char *path)
{
	internal_Init();
	read(path);
}
CImageIO::CImageIO(int width, int height, AVPixelFormat inPIX_FMT, uint8_t *inRawData, int rawLength, bool copy_raw_data)
{
	internal_Init();
	setupImageIO(width, height, inPIX_FMT, inRawData, rawLength, copy_raw_data);
}
void CImageIO::setupImageIO(int width, int height, AVPixelFormat inPIX_FMT, uint8_t *inRawData, int rawLength, bool copy_raw_data)
{
	this->width = width;
	this->height = height;
	this->inPixelFormat = inPIX_FMT;
	raw_length = rawLength;
	if (copy_raw_data)
	{
		if (raw_data)
			delete[] raw_data;

		raw_data = new uint8_t[raw_length];
		memcpy(raw_data, inRawData, raw_length);
	}
	else
		raw_data = inRawData;
}
bool CImageIO::write(const char *path, int destW, int destH)
{
	int got_ptr;
	AVFrame *picFrame, *scaledFrame;
	AVPacket pkt;
	AVCodecID image_codecID;
	AVFormatContext *pOutFormat = NULL;
	FILE *outfile;
	if (width <= 0 || height <= 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:You should set the width & height before writing image.\n");
		return false;
	}
	outfile = fopen(path, "wb");
	if (!outfile)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:[%s] is invalid to write image.\n", path);
		return false;
	}
	pOutFormat = avformat_alloc_context();
	pOutFormat->oformat = av_guess_format(NULL, path, 0);
	if (!pOutFormat->oformat)
	{
		av_log(0, AV_LOG_FATAL, "This format is not supported.\n", path);
		return false;
	}
	image_codecID = av_guess_codec(pOutFormat->oformat, 0, path, 0, AVMEDIA_TYPE_VIDEO);
	pCodec = avcodec_find_encoder(image_codecID);
	if (!pCodec)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not find image encoder.\n");
		return false;
	}
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not allocate context of image encoder.\n");
		return false;
	}
	pCodecCtx->pix_fmt = *pCodec->pix_fmts;
	pCodecCtx->codec_type = pCodec->type;
	pCodecCtx->time_base = { 1, 1 };
	if (!destW || !destH)
	{
		destW = width;
		destH = height;
	}
	pCodecCtx->width = destW;
	pCodecCtx->height = destH;

	pSwsContext = sws_getCachedContext(0, width, height, inPixelFormat, destW, destH,
		pCodecCtx->pix_fmt, SWS_BILINEAR, 0, 0, 0);

	if (avcodec_open2(pCodecCtx, pCodec, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not initialize image codec.\n");
		return false;
	}
	picFrame = CImageFormatConverter::makePicture(width, height, inPixelFormat, raw_data);
	scaledFrame = CImageFormatConverter::makePicture(destW, destH, pCodecCtx->pix_fmt);
	sws_scale(pSwsContext, picFrame->data, picFrame->linesize,
		0, picFrame->height, scaledFrame->data, scaledFrame->linesize);
	av_init_packet(&pkt);
	pkt.data = 0;
	pkt.size = 0;
	avcodec_encode_video2(pCodecCtx, &pkt, scaledFrame, &got_ptr);
	if (got_ptr)
		fwrite(pkt.data, 1, pkt.size, outfile);
	else
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Encoding error.");
	}
	fclose(outfile);
	sws_freeContext(pSwsContext);
	avcodec_free_context(&pCodecCtx);
	avformat_free_context(pOutFormat);
	av_frame_free(&picFrame);
	av_freep(scaledFrame->data);
	av_frame_free(&scaledFrame);
	av_packet_unref(&pkt);
	return true;
}
bool CImageIO::write(FILE *outfile, const char *fileExtension, int destW, int destH)
{
	int got_ptr;
	AVFrame *picFrame, *scaledFrame;
	AVPacket pkt;
	AVCodecID image_codecID;
	AVFormatContext *pOutFormat = NULL;
	if (width <= 0 || height <= 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:You should set the width & height before writing image.\n");
		return false;
	}
	if (!outfile)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO: invalid to write image.\n");
		return false;
	}
	pOutFormat = avformat_alloc_context();
	pOutFormat->oformat = av_guess_format(NULL, fileExtension, 0);
	if (!pOutFormat->oformat)
	{
		av_log(0, AV_LOG_FATAL, "This format is not supported.\n", fileExtension);
		return false;
	}
	image_codecID = av_guess_codec(pOutFormat->oformat, 0, fileExtension, 0, AVMEDIA_TYPE_VIDEO);
	pCodec = avcodec_find_encoder(image_codecID);
	if (!pCodec)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not find image encoder.\n");
		return false;
	}
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not allocate context of image encoder.\n");
		return false;
	}
	pCodecCtx->pix_fmt = *pCodec->pix_fmts;
	pCodecCtx->codec_type = pCodec->type;
	pCodecCtx->time_base = { 1, 1 };
	if (!destW || !destH)
	{
		destW = width;
		destH = height;
	}
	pCodecCtx->width = destW;
	pCodecCtx->height = destH;

	pSwsContext = sws_getCachedContext(0, width, height, inPixelFormat, destW, destH,
		pCodecCtx->pix_fmt, SWS_BILINEAR, 0, 0, 0);

	if (avcodec_open2(pCodecCtx, pCodec, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not initialize image codec.\n");
		return false;
	}
	picFrame = CImageFormatConverter::makePicture(width, height, inPixelFormat, raw_data);
	scaledFrame = CImageFormatConverter::makePicture(destW, destH, pCodecCtx->pix_fmt);
	sws_scale(pSwsContext, picFrame->data, picFrame->linesize,
		0, picFrame->height, scaledFrame->data, scaledFrame->linesize);
	av_init_packet(&pkt);
	pkt.data = 0;
	pkt.size = 0;
	avcodec_encode_video2(pCodecCtx, &pkt, scaledFrame, &got_ptr);
	if (got_ptr)
		fwrite(pkt.data, 1, pkt.size, outfile);
	else
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Encoding error.");
	}

	fclose(outfile);
	sws_freeContext(pSwsContext);
	avcodec_free_context(&pCodecCtx);
	avformat_free_context(pOutFormat);
	av_frame_free(&picFrame);
	av_freep(scaledFrame->data);
	av_frame_free(&scaledFrame);
	av_packet_unref(&pkt);

	return true;
}
bool CImageIO::writeRawData(const char *path)
{
	FILE *outfile;
	outfile = fopen(path, "wb");
	if (!outfile)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:[%s] is invalid to write raw data.\n", path);
		return false;
	}
	fwrite(raw_data, 1, raw_length, outfile);
	fclose(outfile);
	return true;
}
bool CImageIO::read(const char *path)
{
	int ret;
	AVFormatContext *pInFormat = 0;
	AVPacket pkt;
	AVFrame *inFrame, *outFrame;
	AVCodecContext *p;
	int got_picture;
	if (avformat_open_input(&pInFormat, path, 0, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not open the specified file from [%s]", path);
		return false;
	}
	p = pInFormat->streams[0]->codec;
	pCodec = avcodec_find_decoder(p->codec_id);
	pCodecCtx = avcodec_alloc_context3(pCodec);
	avcodec_copy_context(pCodecCtx, p);
	inFrame = av_frame_alloc();
	if (avcodec_open2(pCodecCtx, pCodec, 0) < 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Could not initialize image codec.\n");
		return false;
	}

	while (av_read_frame(pInFormat, &pkt) >= 0)
	{
		ret = avcodec_decode_video2(pCodecCtx, inFrame, &got_picture, &pkt);
		if (ret < 0)
		{
			av_log(0, AV_LOG_FATAL, "CImageIO:Decoding error.\n");
			return false;
		}
		if (got_picture)
		{
			pSwsContext = sws_getCachedContext(0, inFrame->width, inFrame->height, pCodecCtx->pix_fmt,
				pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, SWS_BILINEAR, 0, 0, 0);

			outFrame = CImageFormatConverter::makePicture(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 0);


			sws_scale(pSwsContext, inFrame->data, inFrame->linesize, 0, inFrame->height, outFrame->data, outFrame->linesize);

			setupImageIO(outFrame->width, outFrame->height, pCodecCtx->pix_fmt, outFrame->data[0],
				CImageFormatConverter::getPictureSize(outFrame->width, outFrame->height, pCodecCtx->pix_fmt));

			av_freep(outFrame->data);
			av_frame_free(&outFrame);

			break;
		}
	}
	sws_freeContext(pSwsContext);
	av_frame_free(&inFrame);
	av_frame_free(&outFrame);
	av_packet_unref(&pkt);
	avcodec_free_context(&pCodecCtx);
	avformat_close_input(&pInFormat);
	return true;
}
bool CImageIO::readRawData(const char *path, int width, int height, int raw_length, AVPixelFormat inPIX_FMT)
{
	FILE *inFile;
	uint8_t *raw_data;
	inFile = fopen(path, "rb");
	if (!inFile)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:CImageIO:Could not open the specified file from [%s]\n", path);
		return true;
	}
	raw_data = new uint8_t[raw_length];
	fread(raw_data, 1, raw_length, inFile);
	fclose(inFile);
	setupImageIO(width, height, inPIX_FMT, raw_data, raw_length);
	delete[] raw_data;
	return true;
}
void CImageIO::scale(AVPixelFormat newFmt)
{
	scale(width, height, newFmt);
}
void CImageIO::scale(int newWidth, int newHeight, AVPixelFormat newFmt)
{
	AVFrame *inFrame, *newFrame;
	if (width <= 0 || height <= 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Didn't initialize anything!\n");
		return;
	}
	if (newWidth <= 0 || newHeight <= 0)
	{
		av_log(0, AV_LOG_FATAL, "CImageIO:Passing a valid width & height.\n");
		return;
	}
	if (newFmt == AV_PIX_FMT_NONE)
		newFmt = inPixelFormat;
	pSwsContext = sws_getCachedContext(0, width, height, inPixelFormat, newWidth, newHeight,
		newFmt, SWS_BILINEAR, 0, 0, 0);

	inFrame = CImageFormatConverter::makePicture(width, height, inPixelFormat, raw_data);
	newFrame = CImageFormatConverter::makePicture(newWidth, newHeight, newFmt);
	sws_scale(pSwsContext, inFrame->data, inFrame->linesize, 0,
		inFrame->height, newFrame->data, newFrame->linesize);
	setupImageIO(newWidth, newHeight, newFmt, newFrame->data[0],
		CImageFormatConverter::getPictureSize(newWidth, newHeight, newFmt));
	sws_freeContext(pSwsContext);
	av_frame_free(&inFrame);
	av_frame_free(&newFrame);
}
PIXEL CImageIO::getPixel(int x, int y)
{
	int len;
	PIXEL out = { 0 };
	if (inPixelFormat != AV_PIX_FMT_BGR24)
	{
		av_log(0, AV_LOG_FATAL, "GetPixel function is unable to use with type wasn't AV_PIX_FMT_BGR24");
		return out;
	}
	else
	{
		len = 3 * (width * y + x);
		out.b = raw_data[len];
		out.g = raw_data[len + 1];
		out.r = raw_data[len + 2];
		return out;
	}
}
bool CImageIO::setPixel(int x, int y, const PIXEL &p)
{
	int len;
	if (inPixelFormat != AV_PIX_FMT_BGR24)
	{
		av_log(0, AV_LOG_FATAL, "SetPixel function is unable to use with type wasn't AV_PIX_FMT_BGR24");
		return false;
	}
	else
	{
		len = 3 * (width * y + x);
		raw_data[len] = p.b;
		raw_data[len + 1] = p.g;
		raw_data[len + 2] = p.r;
		return true;
	}
}
bool CImageIO::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	int len;
	if (inPixelFormat != AV_PIX_FMT_BGR24)
	{
		av_log(0, AV_LOG_FATAL, "SetPixel function is unable to use with type wasn't AV_PIX_FMT_BGR24");
		return false;
	}
	else
	{
		len = 3 * (width * y + x);
		raw_data[len] = b;
		raw_data[len + 1] = g;
		raw_data[len + 2] = r;
		return true;
	}
}
int CImageIO::getWidth()
{
	return width;
}
int CImageIO::getHeight()
{
	return height;
}
void CImageIO::getSize(int &w, int &h)
{
	w = width;
	h = height;
}
const uint8_t* CImageIO::getRawData()
{
	return raw_data;
}
int CImageIO::getRawDataLength()
{
	return raw_length;
}
void CImageIO::cloneRawData(uint8_t** destRawData)
{
	uint8_t *out;
	out = (uint8_t *)av_malloc(raw_length);
	memcpy(out, raw_data, raw_length);
	*destRawData = out;
}
void CImageIO::showProperties() const
{
	av_log(0, AV_LOG_INFO, "----CImageIO Information----\n");
	av_log(0, AV_LOG_INFO, "\tWidth:%d\n", pCodecCtx->width);
	av_log(0, AV_LOG_INFO, "\tHeight:%d\n", pCodecCtx->height);
}
bool CImageIO::internal_Init()
{
	width = 0;
	height = 0;
	raw_length = 0;
	pSwsContext = NULL;
	raw_data = NULL;
	inPixelFormat = AV_PIX_FMT_NONE;
	return false;
}