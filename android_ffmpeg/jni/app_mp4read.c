//
// Created by weijiantao on 2018/1/22.
//
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/mem.h>
#include <android/log.h>

static jfieldID mp4ReadField;
#define LOG_TAG "Mp4Read"
typedef jboolean bool;
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define AAC_ADTS_HEADER_READ (sizeof(aac_adts_fixed_headerRead) + sizeof(aac_adts_variable_headerRead) - 1)

typedef struct _aac_adts_fixed_headerRead {
    unsigned int _syncword:12;
    unsigned int _id:1;
    unsigned int _layer:2;
    unsigned int _protection_absent:1;
    unsigned int _profile:2;
    unsigned int _sampling_frequency_index:4;
    unsigned int _private_bit:1;
    unsigned int _channel_configuration:3;
    unsigned int _original_copy:1;
    unsigned int _home:1;
} aac_adts_fixed_headerRead;

typedef struct _aac_adts_variable_headerRead {
    unsigned int _copyright_identification_bit:1;
    unsigned int _copyright_identification_start:1;
    unsigned int _aac_frame_length:13;
    unsigned int _adts_buffer_fullness:11;
    unsigned int _numble_of_raw_data_blocks_in_frame:2;
} aac_adts_variable_headerRead;

typedef enum {
    video_codec_id_h264 = 1,
    video_codec_id_h265 = 2,
} video_codec_id;

typedef enum {
    audio_codec_id_aac = 0,
    audio_codec_id_g711a,
} audio_codec_id;

typedef struct _mp4info {
    bool _getfirstframe;
    video_codec_id _video_codec;
    audio_codec_id _audio_codec;
    AVFormatContext *_format_context;
    AVStream *_video_stream;
    unsigned char *_video_extradata;
    unsigned int _video_extradata_size;
    unsigned int _video_frame_index;
    unsigned int _video_fps;
    unsigned int _video_lasttimestamp;
    AVBitStreamFilterContext *_video_bsfc;
    AVStream *_audio_stream;
    unsigned char *_audio_extradata;
    unsigned int _audio_extradata_size;
    unsigned int _audio_frame_index;
} Mp4info;

int aac_rate_to_frequencyRead(int frequency) {
    switch (frequency) {
        case 96000:
            return 0;
        case 44100:
            return 4;
        case 32000:
            return 5;
        case 16000:
            return 8;
        case 8000:
            return 11;
        default:
            break;
    }
    return -1;
}

unsigned char *
imi_make_aac_header_net(unsigned int frequency, unsigned int channel, unsigned int frame_len) {
    unsigned char *aac_header = (unsigned char *) malloc(AAC_ADTS_HEADER_READ);
    unsigned char *aac_header_buff = aac_header;
    memset(aac_header, 0, AAC_ADTS_HEADER_READ);
    *aac_header_buff++ = 0xff;                                                                    //syncword  (0xfff, high_8bits)
    *aac_header_buff = 0xf0;                                                                    //syncword  (0xfff, low_4bits)
    *aac_header_buff |= (1
            << 3);                                                                //ID (0, 1bit)
    *aac_header_buff |= (0
            << 1);                                                                //layer (0, 2bits)
    *aac_header_buff |= 1;                                                                        //protection_absent (1, 1bit)
    aac_header_buff++;
    *aac_header_buff = (unsigned char) ((1 & 0x3)
            << 6);                                            //profile (profile, 2bits)
    *aac_header_buff |= ((aac_rate_to_frequencyRead(frequency) & 0xf)
            << 2);    //sampling_frequency_index (sam_idx, 4bits)
    *aac_header_buff |= (0
            << 1);                                                                //private_bit (0, 1bit)
    *aac_header_buff |= ((channel & 0x4)
            >> 2);                                                    //channel_configuration (channel, high_1bit)
    aac_header_buff++;
    *aac_header_buff = ((channel & 0x3)
            << 6);                                                    //channel_configuration (channel, low_2bits)
    *aac_header_buff |= (0
            << 5);                                                                //original/copy (0, 1bit)
    *aac_header_buff |= (0
            << 4);                                                                //home  (0, 1bit);
    *aac_header_buff |= (0
            << 3);                                                                //copyright_identification_bit (0, 1bit)
    *aac_header_buff |= (0
            << 2);                                                                //copyright_identification_start (0, 1bit)
    *aac_header_buff |= ((frame_len & 0x1800)
            >> 11);                                            //frame_length (value, high_2bits)
    aac_header_buff++;
    *aac_header_buff++ = (unsigned char) ((frame_len & 0x7f8)
            >> 3);                            //frame_length (value, middle_8bits)
    *aac_header_buff = (unsigned char) ((frame_len & 0x7)
            << 5);                                //frame_length (value, low_3bits)
    *aac_header_buff |= 0;                                                                        //adts_buffer_fullness (0x7ff, high_5bits)
    aac_header_buff++;
    *aac_header_buff = 0;                                                                        //adts_buffer_fullness (0x7ff, low_6bits)
    *aac_header_buff |= 0;                                                                        //number_of_raw_data_blocks_in_frame (0, 2bits);
    aac_header_buff++;
    return aac_header;
}

JNIEXPORT void JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Read_nativeInit(JNIEnv *env, jclass type) {
    jclass localClass = (*env)->FindClass(env,
                                          "com/xiaomi/smarthome/fastvideo/decoder/Mp4Read");
    mp4ReadField = (*env)->GetFieldID(env, localClass, "nativeId", "J");
    (*env)->DeleteLocalRef(env, localClass);
    LOGE("nativeInit ");
}

JNIEXPORT void JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Read_init(JNIEnv *env, jobject instance) {
    av_register_all();
    Mp4info *mp4info = (Mp4info *) (*env)->GetLongField(env, instance, mp4ReadField);
    if (mp4info != NULL) {
        memset(mp4info, 0, sizeof(Mp4info));
        return;
    }
    mp4info = (Mp4info *) malloc(sizeof(Mp4info));
    memset(mp4info, 0, sizeof(Mp4info));
    (*env)->SetLongField(env, instance, mp4ReadField, (long) mp4info);
}

int openfile(
        Mp4info *info,
        const char *path,
        jint *video,
        jint *audio,
        jint *vFps_,
        jint *vDur_,
        jint *vWidth_,
        jint *vHeight_,
        jint *aSampleRate_) {
    unsigned int i = 0;
    int ret = avformat_open_input(&info->_format_context, path, NULL, NULL);
    if (ret != 0) {
        LOGE("avformat_open_input error = %d", ret);
        goto error;
    }
    ret = avformat_find_stream_info(info->_format_context, NULL);
    if (ret < 0) {
        LOGE("avformat_find_stream_info error = %d", ret);
        goto error;
    }
    if (strstr((char *) info->_format_context->iformat->extensions, "mp4") == NULL) {
        LOGE("extensions error = %s",
             (char *) info->_format_context->iformat->extensions);
        goto error;
    }
    *vDur_ = (jint) (info->_format_context->duration / AV_TIME_BASE) * 1000;
    for (i = 0; i < info->_format_context->nb_streams; i++) {
        AVStream *stream = info->_format_context->streams[i];
        switch (stream->codec->codec_type) {
            case AVMEDIA_TYPE_VIDEO: {
                switch (stream->codec->codec_id) {
                    case AV_CODEC_ID_H264: {
                        LOGE("video h264");
                        *video = video_codec_id_h264;
                        info->_video_bsfc = av_bitstream_filter_init("h264_mp4toannexb");
                        if (info->_video_bsfc == NULL) {
                            LOGE("av_bitstream_filter_init h264_mp4toannexb error");
                            goto error;
                        }
                    }
                        break;
                    case AV_CODEC_ID_H265: {
                        *video = video_codec_id_h265;
                        LOGE("video h265");
                        info->_video_bsfc = av_bitstream_filter_init("hevc_mp4toannexb");
                        if (info->_video_bsfc == NULL) {
                            LOGE("av_bitstream_filter_init hevc_mp4toannexb error");
                            goto error;
                        }
                    }
                        break;
                    default:
                        LOGE("video stream type default = %d",
                             stream->codec->codec_id);
                        goto error;
                }
                *vFps_ = stream->codec->framerate.num;
                *vWidth_ = stream->codec->width;
                *vHeight_ = stream->codec->height;
            }
                break;
            case AVMEDIA_TYPE_AUDIO: {
                switch (stream->codec->codec_id) {
                    case AV_CODEC_ID_AAC: {
                        *audio = audio_codec_id_aac;
                        LOGE("video aac");
                    }
                        break;
                    case AV_CODEC_ID_PCM_ALAW: {
                        *audio = audio_codec_id_g711a;
                        LOGE("video g711a");
                    }
                        break;
                    default:
                        LOGE("audio stream type default = %d",
                             stream->codec->codec_id);
                        goto error;
                }
                *aSampleRate_ = stream->codec->sample_rate;
            }
                break;
            default:
                continue;
        }
    }
    LOGE("success");
    return 0;
    error:
    if (&info->_video_bsfc) {
        av_bitstream_filter_close(info->_video_bsfc);
        info->_video_bsfc = NULL;
    }
    if (&info->_format_context) {
        avformat_close_input(&info->_format_context);
        info->_format_context = NULL;
    }
    return -1;
}

JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Read_openFile(JNIEnv *env, jobject instance, jstring path_,
                                         jintArray videoCodecID_, jintArray audioCodecID_,
                                         jintArray vFps_, jintArray vDur_,
                                         jintArray vWidth_, jintArray vHeight_,
                                         jintArray aSampleRate_) {
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    jint *videoCodecID = (*env)->GetIntArrayElements(env, videoCodecID_, NULL);
    jint *audioCodecID = (*env)->GetIntArrayElements(env, audioCodecID_, NULL);
    jint *vFps = (*env)->GetIntArrayElements(env, vFps_, NULL);
    jint *vDur = (*env)->GetIntArrayElements(env, vDur_, NULL);
    jint *vWidth = (*env)->GetIntArrayElements(env, vWidth_, NULL);
    jint *vHeight = (*env)->GetIntArrayElements(env, vHeight_, NULL);
    jint *aSampleRate = (*env)->GetIntArrayElements(env, aSampleRate_, NULL);
    Mp4info *mp4info = (Mp4info *) (*env)->GetLongField(env, instance, mp4ReadField);
    int ret = -1;
    if (mp4info != NULL) {
        ret = openfile(mp4info, path, videoCodecID, audioCodecID, vFps, vDur, vWidth, vHeight,
                       aSampleRate);
    }
    (*env)->ReleaseStringUTFChars(env, path_, path);
    (*env)->ReleaseIntArrayElements(env, videoCodecID_, videoCodecID, 0);
    (*env)->ReleaseIntArrayElements(env, audioCodecID_, audioCodecID, 0);
    (*env)->ReleaseIntArrayElements(env, vFps_, vFps, 0);
    (*env)->ReleaseIntArrayElements(env, vDur_, vDur, 0);
    (*env)->ReleaseIntArrayElements(env, vWidth_, vWidth, 0);
    (*env)->ReleaseIntArrayElements(env, vHeight_, vHeight, 0);
    (*env)->ReleaseIntArrayElements(env, aSampleRate_, aSampleRate, 0);
    return ret;
}

int getVideoFrame(Mp4info *info, unsigned char *data, jint *timestamp, jint *isKeyFrame,
                  jint *isAudioFrame) {
    int ret = 0;
    AVStream *stream = NULL;
    AVPacket pkt = {0};
    if (info->_format_context == NULL) {
        LOGE("imimp4_get_video_frame format_context is null");
        return -1;
    }
    if (*timestamp != 0) {
        LOGE("imimp4_get_video_frame av_seek_frame timestamp = %d", *timestamp);
        int64_t time = (int64_t) (((double) (*timestamp) / (double) 1000) * AV_TIME_BASE +
                                  (double) info->_format_context->start_time);
        int r = av_seek_frame(info->_format_context, -1, time,
                              AVSEEK_FLAG_BACKWARD);//AVSEEK_FLAG_BACKWARD
        if (r < 0) {
            LOGE("av_seek_frame error = %d", r);
            return r;
        }
    }
    av_init_packet(&pkt);
    ret = av_read_frame(info->_format_context, &pkt);
    if (ret < 0 || pkt.size == 0) {
        LOGE("av_read_frame end ret = %d", ret);
        av_free_packet(&pkt);
        return -2;
    }
    int data_len = 0;
    stream = info->_format_context->streams[pkt.stream_index];
    switch (stream->codec->codec_type) {
        case AVMEDIA_TYPE_VIDEO: {
            *isAudioFrame = 0;//false
            if (pkt.flags & AV_PKT_FLAG_KEY) {
                *isKeyFrame = 1;//true
            } else {
                *isKeyFrame = 0;//false
            }
            if (info->_video_bsfc) {
                unsigned char *poutbuf = NULL;
                int poutbuf_size = 0;
                ret = av_bitstream_filter_filter(info->_video_bsfc, stream->codec, NULL,
                                                 (uint8_t **) &poutbuf, &poutbuf_size,
                                                 (uint8_t *) pkt.data, pkt.size,
                                                 pkt.flags & AV_PKT_FLAG_KEY);
                if (ret >= 0) {
                    if (pkt.flags & AV_PKT_FLAG_KEY) {
                        data_len = poutbuf_size;
                    } else {
                        data_len = pkt.size;
                    }
                    if (data_len > 1024 * 1024 || poutbuf_size > 1024 * 1024) {
                        LOGE("data_len too big %d===%d", data_len, poutbuf_size);
                        av_free(poutbuf);
                        return 0;
                    }
                    memcpy(data, poutbuf, poutbuf_size);
                    av_free(poutbuf);
                } else {
                    data_len = pkt.size;
                    memcpy(data, (unsigned char *) pkt.data, pkt.size);
                }
            } else {
                data_len = pkt.size;
                memcpy(data, (unsigned char *) pkt.data, pkt.size);
            }
        }
            break;
        case AVMEDIA_TYPE_AUDIO: {
            *isAudioFrame = 1;//true
            if (stream->codec->codec_id == AV_CODEC_ID_AAC) {
                unsigned char *aac_header = imi_make_aac_header_net(stream->codec->sample_rate,
                                                                    stream->codec->channels,
                                                                    pkt.size +
                                                                    AAC_ADTS_HEADER_READ);
                memcpy(data, aac_header, AAC_ADTS_HEADER_READ);
                free(aac_header);
                data_len = pkt.size + AAC_ADTS_HEADER_READ;
                memcpy(data + AAC_ADTS_HEADER_READ, (unsigned char *) pkt.data, pkt.size);
            } else {
                data_len = pkt.size;
                memcpy(data, (unsigned char *) pkt.data, pkt.size);
            }
        }
            break;
        default:
            break;
    }
    *timestamp = (unsigned int) (((double) pkt.pts / (double) stream->time_base.den) * 1000);
    av_free_packet(&pkt);
    return data_len;
}


JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Read_getVideoFrame(JNIEnv *env, jobject instance, jbyteArray data_,
                                              jintArray vTimesTamp_, jintArray isKeyFrame_,
                                              jintArray isAudioFrame_) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    jint *vTimesTamp = (*env)->GetIntArrayElements(env, vTimesTamp_, NULL);
    jint *isKeyFrame = (*env)->GetIntArrayElements(env, isKeyFrame_, NULL);
    jint *isAudioFrame = (*env)->GetIntArrayElements(env, isAudioFrame_, NULL);
    int ret = -1;
    Mp4info *mp4info = (Mp4info *) (*env)->GetLongField(env, instance, mp4ReadField);
    if (mp4info != NULL) {
        ret = getVideoFrame(mp4info, (unsigned char *) data, vTimesTamp, isKeyFrame, isAudioFrame);
    }
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
    (*env)->ReleaseIntArrayElements(env, vTimesTamp_, vTimesTamp, 0);
    (*env)->ReleaseIntArrayElements(env, isKeyFrame_, isKeyFrame, 0);
    (*env)->ReleaseIntArrayElements(env, isAudioFrame_, isAudioFrame, 0);
    return ret;
}


JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Read_closeFile(JNIEnv *env, jobject instance) {
    Mp4info *mp4info = (Mp4info *) (*env)->GetLongField(env, instance, mp4ReadField);
    if (mp4info == NULL) {
        return -1;
    }
    if (mp4info->_video_bsfc) {
        av_bitstream_filter_close(mp4info->_video_bsfc);
        mp4info->_video_bsfc = NULL;
    }
    if (mp4info->_format_context) {
        avformat_close_input(&mp4info->_format_context);
        mp4info->_format_context = NULL;
    }
    LOGE("success");
    return 0;
}
