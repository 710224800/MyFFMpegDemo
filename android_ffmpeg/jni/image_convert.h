/*
 * image_convert.h
 *
 *  Created on: 2014年8月6日
 *      Author: livy
 */

#ifndef IMAGE_CONVERT_H_
#define IMAGE_CONVERT_H_

typedef unsigned char U8;

/**
YUV420P，Y，U，V三个分量都是平面格式，分为I420和YV12。I420格式和YV12格式的不同处在U平面和V平面的位置不同。在I420格式中，U平面紧跟在Y平面之后，然后才是V平面（即：YUV）；但YV12则是相反（即：YVU）。
YUV420SP, Y分量平面格式，UV打包格式, 即NV12。 NV12与NV21类似，U 和 V 交错排列,不同在于UV顺序。
I420: YYYYYYYY UU VV    =>YUV420P
YV12: YYYYYYYY VV UU    =>YUV420P
NV12: YYYYYYYY UVUV     =>YUV420SP
NV21: YYYYYYYY VUVU     =>YUV420SP
*/

//NV12: YYYYYYYY UVUV     =>YUV420SP
void NV12RGB(U8* yuv420sp, U8* rgb, int width, int height);
//NV21: YYYYYYYY VUVU     =>YUV420SP
void NV21RGB(U8* yuv420sp, U8* rgb, int width, int height);
//YV12: YYYYYYYY VV UU    =>YUV420P
void YV12RGB(U8* yuv420sp, U8* rgb, int width, int height);
//I420: YYYYYYYY UU VV    =>YUV420P
void I420RGB(U8* yuv420sp, U8* rgb, int width, int height);

void FFMPEG_YUV420P2RGB(U8* yuv420sp[],int linesize[], U8* rgb, int width, int height);

void YUV420PBuffer2RGB(U8* yuvdata, U8* rgb, int offset[], int stride[],int width,int height,int type);
void NV21Buffer2RGB(U8* yuvdata, U8* rgb, int offset[], int stride[],int width,int height,int type);
#endif /* IMAGE_CONVERT_H_ */
