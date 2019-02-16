#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#include "FFmpegBase.h"
#define RGB_TO_YUV420P 1
#define YUV420P_TO_RGB 2
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}PIXEL;
class CImageFormatConverter : CFFmpegCodecBase
{
public:
	CImageFormatConverter();
	CImageFormatConverter(int srcW, int srcH, int destW,int destH,
		AVPixelFormat inPIX_FMT,AVPixelFormat outPIX_FMT);
	/*
		---Call this function before trying to convert anything---
	*/
	bool				 setupConverter(int srcW, int srcH, int destW, int destH,
							AVPixelFormat inPIX_FMT, AVPixelFormat outPIX_FMT);
	void				 convert(uint8_t *srcData, AVFrame **out);
	/*
		---Important---
		Note that, This function only supported RGB24->YUV420P now
		If you are trying to pass the other format, 
		I don't guarantee this function will work properly.
	*/
	void				 inv_convert(uint8_t *srcData, AVFrame **out);
	/*
		---Use for creating AVFrame data---
		You can use this function to create AVFrame by passing raw image data.
	*/
	static AVFrame*		 makePicture(int w = 0, int h = 0, AVPixelFormat pixelFmt = AV_PIX_FMT_NONE, uint8_t *data = nullptr);
	static int			 getPictureSize(int w, int h, AVPixelFormat pixelFmt);
	/*
		---Easy to use---
		Use this function to free AVFrame data.
	*/	
	static void			freePicture(AVFrame **pFrame);
	void				showProperties() const final;
private:
	bool				internal_Init() final;
	int					srcWidth;
	int					srcHeight;
	int					destWidth;
	int					destHeight;
	int					inv_inlineSize[1];
	int					inv_srcSlice_val[1];
	AVFrame			   *outPicture;
	uint8_t			   *pFrameOutBuf;
	SwsContext         *pSwsCtx;
	AVPixelFormat	    inPixelFormat;
	AVPixelFormat	    outPixelFormat;
	bool				isInitializedIMGCvt;
};
class CImageIO : CFFmpegCodecBase
{
public:
	CImageIO();
	~CImageIO();
	CImageIO(const char *path);
	CImageIO(int width, int height, AVPixelFormat inPIX_FMT, uint8_t *inRawData, int rawLength,bool copy_raw_data = true);
	/*
		---Important---
		Use this function to setup the image io object,
		you may use this function to initialize object by passing raw data.
		However, you could use the other contructor to initailize object itself.
	*/
	void			 setupImageIO(int width, int height, AVPixelFormat inPIX_FMT, uint8_t *inRawData,
								   int rawLength, bool copy_raw_data = true);
	/*
		---Common use---
		This function could determine what header should be used from filename extension.
		Such as "out.png" the write function will output automatically.
		Note that, 'gif' file type is not supported...
	*/
	bool			 write(const char *path,int destW = 0,int destH = 0);
	bool			 write(FILE *file, const char *fileExtension,int destW = 0, int destH = 0);
	/*
		---Common use---
		This function is used for output raw data directly without file header.
		Such as yuv raw data, rgb raw data and etc...
	*/
	bool             writeRawData(const char *path);
	/*
		---Common use---
		This function is used for reading image for specified location.
		Also, it could determine what file type should be used itself.
	*/
	bool			 read(const char *path);
	/*
		---Common use---
        This function is used for reading raw data directly without file header.
		Such as yuv raw data, rgb raw data and etc...
	*/
	bool			 readRawData(const char *path, int width, int height, int raw_length, AVPixelFormat inPIX_FMT);
	/*
		---Common use---
		This function is used for scaling dimension of image 
		or mapping into another raw data type.
	*/
	void			 scale(AVPixelFormat newFmt = AV_PIX_FMT_NONE);
	void			 scale(int newWidth, int newHeight,AVPixelFormat newFmt = AV_PIX_FMT_NONE);
	/*
		---Important---
		Note that, manipulate pixel function is only supported for AV_PIX_FMT_BGR24,
		Before invoking those functions, you should use scale to transform the pixel type.
	*/
	PIXEL			 getPixel(int x, int y);
	bool			 setPixel(int x, int y, const PIXEL &p);
	bool			 setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
	/*
		---Important---
		This function is used to get raw data pointer for object.
		So if you trying to free out the raw data will occur unexpected behavior.
	*/
	const uint8_t*   getRawData();
	int				 getRawDataLength();
	void			 cloneRawData(uint8_t** destRawData);
	int				 getWidth();
	int				 getHeight();
	void			 getSize(int &w, int &h);
	void			 showProperties() const final;
private:
	bool			 internal_Init();
	SwsContext		*pSwsContext;
	int				 width;
	int				 height;
	uint8_t			*raw_data;
	int				 raw_length;
	AVPixelFormat	 inPixelFormat;
};
#endif