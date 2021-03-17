#include <jni.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <android/bitmap.h>

#define LOG_TAG "h264decoder"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static jfieldID nativeIdField;
static jfieldID nativeIdFieldType;
typedef unsigned char U8;

typedef struct _DecoderData {
    struct AVCodec *codec;
    struct AVCodecContext *codec_ctx;
    struct AVFrame *src_frame;
    struct AVFrame *dst_frame;
    struct SwsContext *convert_ctx;
    int frame_ready;
    int inited;
    int width;
    int height;
    unsigned char *buffer;
} DecoderData;

/*
static void av_log_callback(void *ptr, int level, const char *fmt, __va_list vl) {
    static char line[1024] = {0};
    vsnprintf(line, sizeof(line), fmt, vl);
    LOGD("%s", line);
}*/

static void init(DecoderData *decoder, jint type, jboolean multiThread) {
    if (decoder == NULL || decoder->inited == 1)
        return;
    LOGD("h264 init");
    av_register_all();
    if (type == 2) {
        decoder->codec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
    } else {
        decoder->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    }
    decoder->codec_ctx = avcodec_alloc_context3(decoder->codec);
    if (decoder->codec == NULL || decoder->codec_ctx == NULL) {
        LOGE("h264 init fail NULL");
        return;
    }
    if (multiThread) {
        if (type == 2) {
            decoder->codec->capabilities = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_AUTO_THREADS;
            decoder->codec_ctx->active_thread_type = FF_THREAD_FRAME;
            decoder->codec_ctx->thread_count = 0;
        } else {
            decoder->codec->capabilities = AV_CODEC_CAP_DELAY | CODEC_CAP_SLICE_THREADS;
            decoder->codec_ctx->active_thread_type = FF_THREAD_SLICE;
            decoder->codec_ctx->thread_count = av_cpu_count();
            decoder->codec_ctx->flags2 |= CODEC_FLAG_TRUNCATED;
            decoder->codec_ctx->flags2 |= CODEC_FLAG_LOW_DELAY;
        }
        LOGD("support multiple");
    }
    decoder->codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    decoder->codec_ctx->flags2 |= CODEC_FLAG2_CHUNKS;
    decoder->src_frame = av_frame_alloc();
    decoder->dst_frame = av_frame_alloc();
    avcodec_open2(decoder->codec_ctx, decoder->codec, NULL);
    decoder->inited = 1;
}

static void release(DecoderData *decoder) {
    if (decoder == NULL || decoder->inited == 0)
        return;
    LOGD("H264 release");

    if (decoder->src_frame) {
        av_free(decoder->src_frame);
        decoder->src_frame = NULL;
    }
    if (decoder->dst_frame) {
        av_free(decoder->dst_frame);
        decoder->dst_frame = NULL;
    }
    if (decoder->convert_ctx) {
        sws_freeContext(decoder->convert_ctx);
        decoder->convert_ctx = NULL;
    }
    if (decoder->codec_ctx) {
        avcodec_close(decoder->codec_ctx);
        av_free(decoder->codec_ctx);
        decoder->codec_ctx = NULL;
    }
    if (decoder->buffer) {
        free(decoder->buffer);
        decoder->buffer = NULL;
    }
    decoder->inited = 0;

}

/*
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    av_register_all();
    av_log_set_callback(av_log_callback);

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
}*/

JNIEXPORT void Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_nativeInit(JNIEnv *env,
                                                                                  jobject thiz) {
    jclass localClass = (*env)->FindClass(env,
                                          "com/xiaomi/smarthome/fastvideo/decoder/H264Decoder");
    nativeIdField = (*env)->GetFieldID(env, localClass, "nativeId", "J");
    nativeIdFieldType = (*env)->GetFieldID(env, localClass, "videoType", "J");
    (*env)->DeleteLocalRef(env, localClass);
}

JNIEXPORT void Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_init(JNIEnv *env,
                                                                            jobject thiz,
                                                                            jint type,
                                                                            jboolean multi) {
    (*env)->SetLongField(env, thiz, nativeIdFieldType, (long)type);
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder != NULL) {
        return;
    }
    decoder = (DecoderData *) malloc(sizeof(DecoderData));
    if (decoder == NULL) {
        LOGE("malloc DecoderData null");
        return;
    }
    memset(decoder, 0, sizeof(DecoderData));
    (*env)->SetLongField(env, thiz, nativeIdField, (long) decoder);
    init(decoder, type, multi);
    return;

}

JNIEXPORT void Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_release(JNIEnv *env,
                                                                               jobject thiz) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("free DecoderData is null");
        return;
    }
    release(decoder);
    if (decoder != NULL) {
        free(decoder);
        decoder = NULL;
    }
    (*env)->SetLongField(env, thiz, nativeIdField, (long)0);
    (*env)->SetLongField(env, thiz, nativeIdFieldType, (long)0);
}

JNIEXPORT jboolean Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_decode(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  jbyteArray data,
                                                                                  jint num_bytes,
                                                                                  jlong pkt_pts) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return JNI_FALSE;
    }

    if (decoder->inited == 0) {
        LOGE("DecoderData not init ");
        jint type = (int)(*env)->GetLongField(env, thiz, nativeIdFieldType);
        init(decoder, type, JNI_TRUE);
    }

    jbyte *buf = (*env)->GetByteArrayElements(env, data, 0);
    AVPacket packet = {.data = (uint8_t *) buf, .size = num_bytes, .pts = pkt_pts};

//    int frameFinished = 0;
    int input = avcodec_send_packet(decoder->codec_ctx, &packet);
    if (input != 0) {
        LOGE("avcodec_send_packet error:%d", input);
        avcodec_receive_frame(decoder->codec_ctx, decoder->src_frame);
        return JNI_FALSE;
    }
    int output = avcodec_receive_frame(decoder->codec_ctx, decoder->src_frame);
//    int res = avcodec_decode_video2(decoder->codec_ctx, decoder->src_frame,
//                                    &frameFinished, &packet);
//    if (res < 0) {
//        LOGE("avcodec_receive_frame error:%d", res);
//    }
    if (output == 0) {
        decoder->frame_ready = 1;
    } else {
        LOGE("avcodec_receive_frame error:%d", output);
    }
//    if (frameFinished)
//        decoder->frame_ready = 1;
    (*env)->ReleaseByteArrayElements(env, data, buf, 0);
    return (jboolean) (output == 0 ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jboolean
Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_decodeBuffer(JNIEnv *env,
                                                                     jobject thiz,
                                                                     jobject buffer,
                                                                     jint num_bytes,
                                                                     jlong pkt_pts) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return JNI_FALSE;
    }

    if (decoder->inited == 0) {
        LOGE("not init");
        jint type = (int)(*env)->GetLongField(env, thiz, nativeIdFieldType);
        init(decoder, type, JNI_TRUE);
    }

    uint8_t *buf = (uint8_t *) (*env)->GetDirectBufferAddress(env, buffer);

    AVPacket packet = {.data = (uint8_t *) buf, .size = num_bytes, .pts =
    pkt_pts};

    int frameFinished = 0;
    int res = avcodec_decode_video2(decoder->codec_ctx, decoder->src_frame,
                                    &frameFinished, &packet);
    if (res < 0) {
        LOGE("avcodec_decode_video2 error:%d", res);
    }

    if (frameFinished)
        decoder->frame_ready = 1;

    return decoder->frame_ready ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_getWidth(JNIEnv *env,
                                                                                jobject thiz) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return 0;
    }
    if (decoder->codec_ctx)
        return decoder->codec_ctx->width;
    else
        return 0;
}

JNIEXPORT jint Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_getHeight(JNIEnv *env,
                                                                                 jobject thiz) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return 0;
    }
    if (decoder->codec_ctx)
        return decoder->codec_ctx->height;
    else
        return 0;
}

JNIEXPORT jint Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_toYUV(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jbyteArray byte) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return -1;
    }

    if (!decoder->frame_ready) {
        LOGE("DecoderData not ready");
        return -1;
    }

    if (!decoder->codec_ctx || !decoder->src_frame
        || !decoder->src_frame->data[0] || !decoder->src_frame->data[1]
        || !decoder->src_frame->data[2])
        return -1;
    if (decoder->width != decoder->codec_ctx->width
        || decoder->height != decoder->codec_ctx->height) {
        if (decoder->buffer) {
            free(decoder->buffer);
            decoder->buffer = 0;
        }
    }

    decoder->width = decoder->codec_ctx->width;
    decoder->height = decoder->codec_ctx->height;

    jbyte *pByte = (*env)->GetByteArrayElements(env, byte, 0);
    U8 *dst = (U8 *) pByte;
    U8 *src = decoder->src_frame->data[0];
    for (int i = 0; i < decoder->height; i++) {
        memcpy(dst, src, decoder->width);
        dst += decoder->width;
        src += decoder->src_frame->linesize[0];
    }

    int halfH = decoder->height / 2;
    int halfW = decoder->width / 2;
    //u
    src = decoder->src_frame->data[1];
    for (int i = 0; i < halfH; i++) {
        memcpy(dst, src, halfW);
        dst += halfW;
        src += decoder->src_frame->linesize[1];
    }

    //v
    src = decoder->src_frame->data[2];
    for (int i = 0; i < halfH; i++) {
        memcpy(dst, src, halfW);
        dst += halfW;
        src += decoder->src_frame->linesize[2];
    }
    (*env)->ReleaseByteArrayElements(env, byte, pByte, 0);
    return 0;
}

JNIEXPORT jint Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_toTexture(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jint textureY,
                                                                                 jint textureU,
                                                                                 jint textureV) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return -1;
    }

    if (!decoder->frame_ready) {
        LOGE("DecoderData not ready");
        return -1;
    }

    if (!decoder->codec_ctx || !decoder->src_frame
        || !decoder->src_frame->data[0] || !decoder->src_frame->data[1]
        || !decoder->src_frame->data[2]) {
        LOGE("DecoderData data null");
        return -1;
    }

    if (decoder->width != decoder->codec_ctx->width
        || decoder->height != decoder->codec_ctx->height) {
        if (decoder->buffer) {
            free(decoder->buffer);
            decoder->buffer = 0;
        }
    }

    decoder->width = decoder->codec_ctx->width;
    decoder->height = decoder->codec_ctx->height;
    if (!decoder->buffer) {
        decoder->buffer = malloc(
                decoder->codec_ctx->width * decoder->codec_ctx->height);
    }
    if (!decoder->buffer)
        return -1;
//	LOGE("width:%d height:%d linesize:%d", codec_ctx->width, codec_ctx->height,
//			src_frame->linesize[0]);

    //y
    unsigned char *dst = decoder->buffer;
    unsigned char *src = decoder->src_frame->data[0];
    for (int i = 0; i < decoder->height; i++) {
        memcpy(dst, src, decoder->width);
        dst += decoder->width;
        src += decoder->src_frame->linesize[0];
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, decoder->width,
                 decoder->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 decoder->buffer);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int halfH = decoder->height / 2;
    int halfW = decoder->width / 2;
    //u
    dst = decoder->buffer;
    src = decoder->src_frame->data[1];
    for (int i = 0; i < halfH; i++) {
        memcpy(dst, src, halfW);
        dst += halfW;
        src += decoder->src_frame->linesize[1];
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, halfW, halfH, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE, decoder->buffer);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //v
    dst = decoder->buffer;
    src = decoder->src_frame->data[2];
    for (int i = 0; i < halfH; i++) {
        memcpy(dst, src, halfW);
        dst += halfW;
        src += decoder->src_frame->linesize[2];
    }
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, halfW, halfH, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE, decoder->buffer);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return 0;
}

JNIEXPORT jint Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_toBitmap(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jobject bitmap) {
    DecoderData *decoder = (DecoderData *) (*env)->GetLongField(env, thiz, nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return -1;
    }
    if (!decoder->frame_ready)
        return -1;

    if (bitmap == NULL) {
        LOGE("Error bitmap null");
        return -1;
    }

    void *pixels;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        LOGE("Error AndroidBitmap_getInfo");
        return -1; //
    }

    if (info.width != decoder->codec_ctx->width
        || info.height != decoder->codec_ctx->height) {
        LOGE("Error bitmap width:%d,height:%d,frame widht:%d,height:%d",
             info.width, info.height, decoder->codec_ctx->width,
             decoder->codec_ctx->height);
        return -1;
    }

    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        LOGE("Error AndroidBitmap_lockPixels");
        return -1;
    }
    int pixfmt = 0;
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
//		FFMPEG_YUV420P2RGB(src_frame->data, src_frame->linesize, pixels,
//				codec_ctx->width, codec_ctx->height);
        pixfmt = AV_PIX_FMT_RGBA;
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        pixfmt = AV_PIX_FMT_RGB565LE;
    }
    if (decoder->convert_ctx
        && (decoder->width != decoder->codec_ctx->width
            || decoder->height != decoder->codec_ctx->height)) {
        sws_freeContext(decoder->convert_ctx);
        decoder->convert_ctx = NULL;
    }
    if (decoder->convert_ctx == NULL) {
        decoder->convert_ctx = sws_getContext(decoder->codec_ctx->width,
                                              decoder->codec_ctx->height,
                                              decoder->codec_ctx->pix_fmt,
                                              decoder->codec_ctx->width, decoder->codec_ctx->height,
                                              pixfmt,
                                              SWS_FAST_BILINEAR, NULL, NULL, NULL);
        decoder->width = decoder->codec_ctx->width;
        decoder->height = decoder->codec_ctx->height;
    }
//		convert_ctx = sws_getCachedContext(convert_ctx, codec_ctx->width,
//				codec_ctx->height, codec_ctx->pix_fmt, codec_ctx->width,
//				codec_ctx->height, PIX_FMT_RGB565LE, SWS_FAST_BILINEAR, NULL,
//				NULL, NULL);
    if (decoder->convert_ctx == NULL) {
        LOGE("convert_ctx==NULL");
        AndroidBitmap_unlockPixels(env, bitmap);
        return -1;
    }

    int ret = avpicture_fill((AVPicture *) decoder->dst_frame, (uint8_t *) pixels,
                             pixfmt, decoder->codec_ctx->width, decoder->codec_ctx->height);
    if (ret < 0) {
        LOGE("avpicture_fill error:%d", ret);
    } else {
        sws_scale(decoder->convert_ctx,
                  (const uint8_t **) decoder->src_frame->data,
                  decoder->src_frame->linesize, 0, decoder->codec_ctx->height,
                  decoder->dst_frame->data, decoder->dst_frame->linesize);
    }
    decoder->frame_ready = 0;
    if (decoder->src_frame->pkt_pts == AV_NOPTS_VALUE) {
        LOGE("No PTS was passed from avcodec_decode!");
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}

JNIEXPORT jint Java_com_xiaomi_smarthome_fastvideo_decoder_H264Decoder_decodeIFrame(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jbyteArray data,
                                                                                    jint num_bytes,
                                                                                    jint width,
                                                                                    jint height,
                                                                                    jint type,
                                                                                    jobject bitmap) {
    void *pixels;
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        LOGE("Error AndroidBitmap_getInfo");
        return -1; //
    }

    if (info.width != width || info.height != height) {
        LOGE("Error bitmap width:%d,height:%d,frame widht:%d,height:%d",
             info.width, info.height, width, height);
        return -1;
    }

    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        LOGE("Error AndroidBitmap_lockPixels");
        return -1;
    }
    int pixfmt = 0;
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        pixfmt = AV_PIX_FMT_RGBA;
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        pixfmt = AV_PIX_FMT_RGB565LE;
    }

    struct SwsContext *context = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height,
                                                pixfmt,
                                                SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (context == NULL) {
        LOGE("convert_ctx==NULL");
        AndroidBitmap_unlockPixels(env, bitmap);
        return -1;
    }

    DecoderData *decoder = (DecoderData *) malloc(sizeof(DecoderData));

    memset(decoder, 0, sizeof(DecoderData));
    init(decoder, type, JNI_FALSE);

    jbyte *buf = (*env)->GetByteArrayElements(env, data, 0);

    if (buf == NULL) {
        LOGE("data buf is null");
        AndroidBitmap_unlockPixels(env, bitmap);
        release(decoder);
        free(decoder);
        return -1;
    }
    AVPacket packet = {.data = (uint8_t *) buf, .size = num_bytes, .pts =
    0};
    int frameFinished = 0;
    int res = avcodec_decode_video2(decoder->codec_ctx, decoder->src_frame,
                                    &frameFinished, &packet);
    if (res < 0) {
        LOGE("avcodec_decode_video2 error:%d", res);
        AndroidBitmap_unlockPixels(env, bitmap);
        release(decoder);
        free(decoder);
        return res;
    }
    if (decoder->src_frame->key_frame == 1 && frameFinished) {
        AVFrame *dstFrame = NULL;
        dstFrame = av_frame_alloc();
        int ret_d = avpicture_fill((AVPicture *) dstFrame, (uint8_t *) pixels, pixfmt, width,
                                   height);
        if (ret_d < 0) {
            LOGE("avpicture_fill error:%d", ret_d);
        } else {
            if (ret_d > 0 && dstFrame != NULL) {
                LOGE("sws_scale - %d", decoder->src_frame->linesize);
                LOGE("sws_scale - %d", dstFrame->data);
                LOGE("sws_scale - %d", dstFrame->linesize);

                sws_scale(context, decoder->src_frame, decoder->src_frame->linesize, 0, height,
                          dstFrame->data, dstFrame->linesize);
            }
        }
        if (decoder->src_frame->pkt_pts == AV_NOPTS_VALUE) {
            LOGE("No PTS was passed from avcodec_decode!");
        }
        av_free(dstFrame);
    } else {
        LOGE("frame not the i frame or frame not finish!");
    }
    sws_freeContext(context);
    (*env)->ReleaseByteArrayElements(env, data, buf, 0);
    release(decoder);
    free(decoder);
    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}
