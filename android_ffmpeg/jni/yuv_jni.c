#include <jni.h>
#include "image_convert.h"
#include <android/log.h>

#include <android/bitmap.h>
#define LOG_TAG "YUV"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARNING,LOG_TAG,__VA_ARGS__)

JNIEXPORT void JNICALL Java_com_decoder_xiaomi_YUV2RGB_convertI420RGB(JNIEnv *env,jobject obj,jbyteArray yuv,jobject bitmap) {
	void* pixels;
	U8* pDst;
	U8* psrc = (U8*)(*env)->GetByteArrayElements(env, yuv, 0);
	AndroidBitmapInfo info;
	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
	return; //
	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	return;
	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
	return;
	pDst = (U8*)pixels;
	I420RGB(psrc, pDst, info.width, info.height);
	AndroidBitmap_unlockPixels(env, bitmap);
	(*env)->ReleaseByteArrayElements(env, yuv, psrc, 0);
}

JNIEXPORT void JNICALL Java_com_decoder_xiaomi_YUV2RGB_convertYV12RGB(JNIEnv *env,jobject obj,jbyteArray yuv,jobject bitmap) {
	void* pixels;
	U8* pDst;
	U8* psrc = (U8*)(*env)->GetByteArrayElements(env, yuv, 0);
	AndroidBitmapInfo info;
	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
	return; //
	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	return;
	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
	return;
	pDst = (U8*)pixels;
	YV12RGB(psrc, pDst, info.width, info.height);
	AndroidBitmap_unlockPixels(env, bitmap);
	(*env)->ReleaseByteArrayElements(env, yuv, psrc, 0);
}

JNIEXPORT void JNICALL Java_com_decoder_xiaomi_YUV2RGB_convertNV12RGB(JNIEnv *env,jobject obj,jbyteArray yuv,jobject bitmap) {
	void* pixels;
	U8* pDst;
	U8* psrc = (U8*)(*env)->GetByteArrayElements(env, yuv, 0);
	AndroidBitmapInfo info;
	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
	return; //
	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	return;
	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
	return;
	pDst = (U8*)pixels;
	NV12RGB(psrc, pDst, info.width, info.height);
	AndroidBitmap_unlockPixels(env, bitmap);
	(*env)->ReleaseByteArrayElements(env, yuv, psrc, 0);
}

JNIEXPORT void JNICALL Java_com_decoder_xiaomi_YUV2RGB_convertNV21RGB(JNIEnv *env,jobject obj,jbyteArray yuv,jobject bitmap) {
	void* pixels;
	U8* pDst;
	U8* psrc = (U8*)(*env)->GetByteArrayElements(env, yuv, 0);
	AndroidBitmapInfo info;
	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
	return; //
	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	return;
	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
	return;
	pDst = (U8*)pixels;
	NV21RGB(psrc, pDst, info.width, info.height);
	AndroidBitmap_unlockPixels(env, bitmap);
	(*env)->ReleaseByteArrayElements(env, yuv, psrc, 0);
}

JNIEXPORT void JNICALL Java_com_decoder_xiaomi_YUV2RGB_YUVBuffer2Bitmap(JNIEnv *env,jobject obj,jobject buffer,jintArray offset,jintArray stride,jobject bitmap,jint type) {
	void* pixels;
	U8* pDst;
	U8* psrc = (U8*)(*env)->GetDirectBufferAddress(env, buffer);
	jint* offsetArray=(*env)->GetIntArrayElements(env,offset,NULL);
	jint* strideArray=(*env)->GetIntArrayElements(env,stride,NULL);
	AndroidBitmapInfo info;
	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
	return; //
	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
	return;
	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
	return;
	pDst = (U8*)pixels;
	if(strideArray[2]>0) {
		YUV420PBuffer2RGB(psrc, pDst,offsetArray, strideArray,info.width, info.height,type);
	} else {
		NV21Buffer2RGB(psrc, pDst,offsetArray, strideArray,info.width, info.height,type);
	}

	AndroidBitmap_unlockPixels(env, bitmap);
	(*env)->ReleaseIntArrayElements(env,offset,offsetArray,0);
	(*env)->ReleaseIntArrayElements(env,stride,strideArray,0);
}
