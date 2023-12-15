#ifndef _JPEG_H_
#define _JPEG_H_

#define DCTSIZE	8
#define DCTBLOCKSIZE	64
#define DC_MAX_QUANTED  2047   //量化后DC的最大值
#define DC_MIN_QUANTED  -2048   //量化后DC的最小值

/**
函 数 名  : compress_yuyv_to_jpeg
功能描述  : yuv格式转JPG格式
输入参数  : const unsigned char* yuvbuf:存放YUV格式图像的BUF
unsigned char *dstbuf:存放转换后的JPG图像的BUF
int yuvbufsize:存放YUV格式图像的BUF的大小
int height:图像的高度
int width:图像的宽度        
int quality:图像的质量0-100
输出参数  : dstbuf
返 回 值  : 转换后的JPEG图片的大小
**/
extern int compress_yuyv_to_jpeg( const unsigned char* yuvbuf,
		unsigned char *dstbuf,int yuvbufsize, int width,int height,int quality );

#endif