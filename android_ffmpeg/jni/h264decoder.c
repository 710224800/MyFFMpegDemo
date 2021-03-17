#include <jni.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#define LOG_TAG "h264decoder"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARNING,LOG_TAG,__VA_ARGS__)

static jfieldID nativeIdField;

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

static void init(DecoderData *decoder, jboolean multiThread) {
    if (decoder == NULL || decoder->inited == 1)
        return;
    LOGD("init");
    av_register_all();
    decoder->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (decoder->codec == NULL) {
        LOGE("codec not find");
        return;
    }
    decoder->codec_ctx = avcodec_alloc_context3(decoder->codec);
    if (decoder->codec_ctx == NULL) {
        LOGE("codec_ctx not find");
        return;
    }
    if (multiThread) {
        decoder->codec->capabilities = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_SLICE_THREADS;
        decoder->codec_ctx->active_thread_type = FF_THREAD_SLICE;
        decoder->codec_ctx->thread_count = av_cpu_count();
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

JNIEXPORT void Java_com_decoder_xiaomi_H264Decoder_nativeInit(JNIEnv *env,
                                                              jobject thiz) {
    jclass localClass = (*env)->FindClass(env,
                                          "com/decoder/xiaomi/H264Decoder");
    nativeIdField = (*env)->GetFieldID(env, localClass, "nativeId", "I");
    (*env)->DeleteLocalRef(env, localClass);
}

JNIEXPORT void Java_com_decoder_xiaomi_H264Decoder_init(JNIEnv *env,
                                                        jobject thiz,
                                                        jboolean multiThread) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder != NULL)
        return;
    decoder = (DecoderData *) malloc(sizeof(DecoderData));
    if (decoder == NULL) {
        LOGE("malloc DecoderData null");
        return;
    }
    memset(decoder, 0, sizeof(DecoderData));
    (*env)->SetIntField(env, thiz, nativeIdField, (int) decoder);
    init(decoder, multiThread);
    return;

}

JNIEXPORT void Java_com_decoder_xiaomi_H264Decoder_release(JNIEnv *env,
                                                           jobject thiz) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder == NULL) {
        LOGE("free DecoderData is null");
        return;
    }
    release(decoder);
    if (decoder != NULL) {
        free(decoder);
        decoder = NULL;
    }
    (*env)->SetIntField(env, thiz, nativeIdField, 0);
}

JNIEXPORT jboolean Java_com_decoder_xiaomi_H264Decoder_decode(JNIEnv *env,
                                                              jobject thiz, jbyteArray data,
                                                              jint num_bytes, jlong pkt_pts) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return JNI_FALSE;
    }

    if (decoder->inited == 0)
        init(decoder, JNI_TRUE);

    jbyte *buf = (*env)->GetByteArrayElements(env, data, 0);

    AVPacket packet = {.data = (uint8_t *) buf, .size = num_bytes, .pts =
    pkt_pts};

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

JNIEXPORT jboolean Java_com_decoder_xiaomi_H264Decoder_decodeBuffer(JNIEnv *env,
                                                                    jobject thiz, jobject buffer,
                                                                    jint num_bytes, jlong pkt_pts) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return JNI_FALSE;
    }

    if (decoder->inited == 0)
        init(decoder, JNI_TRUE);

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

JNIEXPORT jint Java_com_decoder_xiaomi_H264Decoder_getWidth(JNIEnv *env,
                                                            jobject thiz) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return 0;
    }
    if (decoder->codec_ctx)
        return decoder->codec_ctx->width;
    else
        return 0;
}

JNIEXPORT jint Java_com_decoder_xiaomi_H264Decoder_getHeight(JNIEnv *env,
                                                             jobject thiz) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return 0;
    }
    if (decoder->codec_ctx)
        return decoder->codec_ctx->height;
    else
        return 0;
}

JNIEXPORT jint Java_com_decoder_xiaomi_H264Decoder_toTexture(JNIEnv *env,
                                                             jobject thiz, jint textureY,
                                                             jint textureU, jint textureV) {
    DecoderData *decoder = (DecoderData *) (*env)->GetIntField(env, thiz,
                                                               nativeIdField);
    if (decoder == NULL) {
        LOGE("DecoderData is null");
        return -1;
    }

    if (!decoder->frame_ready)
        return -1;
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
