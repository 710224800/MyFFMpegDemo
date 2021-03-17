//
// Created by weijiantao on 2017/11/28.
//
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <libavformat/avformat.h>
#include <android/log.h>

#define LOG_TAG "Mp4Muxer"

#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define AAC_ADTS_HEADER (sizeof(aac_adts_fixed_header) + sizeof(aac_adts_variable_header) - 1)

typedef enum {
    video_codec_id_h264 = 1,
    video_codec_id_h265 = 2,
} video_muxer_codec_id;

typedef struct _aac_adts_fixed_header {
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
} aac_adts_fixed_header;

typedef struct _aac_adts_variable_header {
    unsigned int _copyright_identification_bit:1;
    unsigned int _copyright_identification_start:1;
    unsigned int _aac_frame_length:13;
    unsigned int _adts_buffer_fullness:11;
    unsigned int _numble_of_raw_data_blocks_in_frame:2;
} aac_adts_variable_header;

static jfieldID mp4MuxerField;

typedef jboolean bool;

typedef struct _MuxerData {
    bool _getfirstframe;
    AVFormatContext *_format_context;
    AVStream *_video_stream;
    unsigned char *_video_extradata;
    unsigned int _video_extradata_size;
    unsigned int _video_frame_index;
    long _video_time;
    int _videoCode;
    AVStream *_audio_stream;
    unsigned char *_audio_extradata;
    unsigned int _audio_extradata_size;
    unsigned int _audio_frame_index;
    int _fps;
} MuxerData;


unsigned char *
imi_make_aac_track_configure(int sampling_frequency_index, int channle) {
//    typedef struct _aac_es_config {
//        unsigned int _resv:3;
//        unsigned int _channel:4;
//        unsigned int _sample:4;
//        unsigned int _type:5;
//    } aac_es_config;
//
//    unsigned char temp[2];
//    unsigned char *trak = (unsigned char *) malloc(2);
//
//    aac_es_config t = {0};
//    t._type = 2;
//    t._sample = 11;
//    t._channel = 1;
//    t._resv = 0;
//
//    memcpy(temp, &t, sizeof(temp));
//    trak[0] = temp[1];
//    trak[1] = temp[0];
    unsigned char *trak = (unsigned char *) malloc(2);
    unsigned int object_type = 2; // AAC LC by default
    trak[0] = (unsigned char) ((object_type << 3) | (sampling_frequency_index >> 1));
    trak[1] = (unsigned char) (((sampling_frequency_index & 1) << 7) | (channle << 3));
    LOGE("aac adt :%d,:%d", trak[0], trak[1]);
    return trak;
}

int aac_rate_to_frequency(int frequency) {
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

int _create_file_inner(MuxerData *handle, const char *path, jint vfps, jint videoCode, jint vwidth, jint vheight, jint asamplerate) {
    int ret = 0;
    if (handle == 0){
        return -1;
    }
    handle->_format_context = avformat_alloc_context();
    if (handle->_format_context == NULL) {
        LOGE("create_file avformat_alloc_context error");
        return -1;
    }
    handle->_fps = vfps;
    handle->_videoCode = videoCode;
    handle->_format_context->oformat = av_guess_format("mp4", path, NULL);
    if (handle->_format_context->oformat == NULL) {
        LOGE("create_file av_guess_format error path = %s", path);
        goto error;
    }

    if (videoCode == video_codec_id_h264) {
        handle->_format_context->oformat->video_codec = AV_CODEC_ID_H264;
    } else {
        handle->_format_context->oformat->video_codec = AV_CODEC_ID_H265;
    }
    handle->_format_context->oformat->audio_codec = AV_CODEC_ID_AAC;

    memcpy(handle->_format_context->filename, path, strlen(path));
    //////////////////////////////////////////////////////////////////////////
    handle->_video_stream = avformat_new_stream(handle->_format_context, NULL);
    if (handle->_video_stream == NULL) {
        LOGE("create_file avformat_new_stream video error");
        goto error;
    }
    handle->_video_stream->id = handle->_format_context->nb_streams - 1;
    handle->_video_stream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    if (handle->_format_context->oformat->flags & AVFMT_GLOBALHEADER) {
        handle->_video_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    if (videoCode == video_codec_id_h264) {
        handle->_video_stream->codec->codec_id = AV_CODEC_ID_H264;
        handle->_video_stream->codec->level = 0x7F;
    } else {
        handle->_video_stream->codec->codec_id = AV_CODEC_ID_H265;
    }
    handle->_video_stream->codec->width = vwidth;
    handle->_video_stream->codec->height = vheight;
    handle->_video_stream->codec->time_base = av_d2q(1.0 / vfps, 255);
    handle->_video_stream->r_frame_rate = av_d2q(vfps, 255);
    handle->_video_stream->time_base.num = 1;
    handle->_video_stream->time_base.den = 90000;
    //////////////////////////////////////////////////////////////////////////
    handle->_audio_stream = avformat_new_stream(handle->_format_context, NULL);
    if (handle->_audio_stream == NULL) {
        LOGE("create_file avformat_new_stream audio error");
        goto error;
    }
    handle->_audio_stream->id = handle->_format_context->nb_streams - 1;
    handle->_audio_stream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    handle->_audio_stream->codec->codec_id = AV_CODEC_ID_AAC;
    handle->_audio_stream->codec->sample_rate = asamplerate;
    //直接使用单声道
    handle->_audio_stream->codec->channels = 1;
    handle->_audio_stream->codec->profile = FF_PROFILE_AAC_LOW;
    handle->_audio_stream->codec->level = 0x02;
    int index = aac_rate_to_frequency(handle->_audio_stream->codec->sample_rate);
    handle->_audio_extradata = imi_make_aac_track_configure(index, 1);
    handle->_audio_extradata_size = 2;
    handle->_audio_stream->codec->extradata = handle->_audio_extradata;
    handle->_audio_stream->codec->extradata_size = handle->_audio_extradata_size;
    handle->_audio_stream->codec->frame_size = 1024;
    handle->_audio_stream->codec->time_base.num = 1;
    handle->_audio_stream->codec->time_base.den = asamplerate;
    handle->_audio_stream->time_base.num = 1;
    handle->_audio_stream->time_base.den = asamplerate;
    //////////////////////////////////////////////////////////////////////////
    av_dump_format(handle->_format_context, 0, path, 1);
    ret = avio_open(&handle->_format_context->pb, path, AVIO_FLAG_WRITE);
    if (ret < 0) {
        LOGE("reate_file avio_open error = %d", ret);
        goto error;
    }
    //////////////////////////////////////////////////////////////////////////
    handle->_video_extradata_size = 0;
    handle->_video_extradata = (unsigned char *) malloc(2048);
    memset(handle->_video_extradata, 0, 2048);
    LOGE("create_file success");
    return 0;

    error:
    if (handle->_video_stream) {
        avcodec_close(handle->_video_stream->codec);
        handle->_video_stream->codec->extradata = NULL;
        handle->_video_stream->codec->extradata_size = 0;
        handle->_video_stream = NULL;
    }
    if (handle->_audio_stream) {
        avcodec_close(handle->_audio_stream->codec);
        handle->_audio_stream->codec->extradata = NULL;
        handle->_audio_stream->codec->extradata_size = 0;
        handle->_audio_stream = NULL;
    }
    avio_close(handle->_format_context->pb);
    handle->_format_context->pb = NULL;
    avformat_free_context(handle->_format_context);
    handle->_format_context = NULL;
    if (handle->_audio_extradata) {
        free(handle->_audio_extradata);
        handle->_audio_extradata = NULL;
    }
    return -1;
}


JNIEXPORT void JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_nativeInit(JNIEnv *env, jclass type) {

    jclass localClass = (*env)->FindClass(env, "com/xiaomi/smarthome/fastvideo/decoder/Mp4Muxer");
    mp4MuxerField = (*env)->GetFieldID(env, localClass, "nativeId", "J");
    (*env)->DeleteLocalRef(env, localClass);
}


JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_initMuxer(JNIEnv *env, jobject instance,
                                                               jstring outPath_,
                                                               jint vFps,
                                                               jint videoCode,
                                                               jint videoWidth, jint videoHeight,
                                                               jint audioSimpleRate) {
    av_register_all();
    const char *outPath = (*env)->GetStringUTFChars(env, outPath_, 0);
    MuxerData *decoder = (MuxerData *) malloc(sizeof(MuxerData));
    if (decoder == NULL) {
        LOGE("malloc DecoderData null");
        (*env)->ReleaseStringUTFChars(env, outPath_, outPath);
        return -1;
    }
    memset(decoder, 0, sizeof(MuxerData));
    (*env)->SetLongField(env, instance, mp4MuxerField, (long) decoder);
    int ret = _create_file_inner(decoder, outPath, vFps, videoCode, videoWidth, videoHeight,
                                 audioSimpleRate);
    if (ret != 0) {
        free(decoder);
        decoder = NULL;
    }
    (*env)->ReleaseStringUTFChars(env, outPath_, outPath);
    return ret;
}


int _write_audio_frame_inner(MuxerData *handle,
                             unsigned char *data,
                             unsigned int data_len) {
    AVPacket pkt = {0};
    unsigned char *data_buff_src = data;
    if (handle->_audio_stream == NULL) {
        LOGE("write_audio_frame audio_stream is null");
        return -1;
    }
    if (handle->_getfirstframe == JNI_FALSE) {
        LOGE("write_audio_frame doesn't' get first keyframe");
        return -2;
    }
    av_init_packet(&pkt);
    pkt.flags |= AV_PKT_FLAG_KEY;
    int header_len = AAC_ADTS_HEADER;
    pkt.data = data_buff_src + header_len;
    pkt.size = data_len - header_len;
    pkt.duration = 1024;
    pkt.stream_index = handle->_audio_stream->index;
    pkt.pts = pkt.duration * (handle->_audio_frame_index++);
    pkt.dts = pkt.pts;
    pkt.pos = -1;
    return av_interleaved_write_frame(handle->_format_context, &pkt);
}

JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_writeAudio(JNIEnv *env, jobject instance,
                                                                jbyteArray data_,
                                                                jint dataLen) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    MuxerData *decoder = (MuxerData *) (*env)->GetLongField(env, instance, mp4MuxerField);
    if (decoder == NULL) {
        (*env)->ReleaseByteArrayElements(env, data_, data, 0);
        LOGE("write audio but muxer null");
        return -1;
    }
    int ret = _write_audio_frame_inner(decoder, data, dataLen);
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
    return ret;
}

int _write_audio_frame_inner_withPts(MuxerData *handle,
                             unsigned char *data,
                             unsigned int data_len,long pts) {
    AVPacket pkt = {0};
    unsigned char *data_buff_src = data;
    if (handle->_audio_stream == NULL) {
        LOGE("write_audio_frame audio_stream is null");
        return -1;
    }
    if (handle->_getfirstframe == JNI_FALSE) {
        LOGE("write_audio_frame doesn't' get first keyframe");
        return -2;
    }
    av_init_packet(&pkt);
    pkt.flags |= AV_PKT_FLAG_KEY;
    int header_len = AAC_ADTS_HEADER;
    pkt.data = data_buff_src + header_len;
    pkt.size = data_len - header_len;
    pkt.stream_index = handle->_audio_stream->index;
    pkt.pts = pts;
    pkt.dts = pkt.pts;
    return av_interleaved_write_frame(handle->_format_context, &pkt);
}

JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_writeAudioWithPts(JNIEnv *env, jobject instance,
                                                                jbyteArray data_,
                                                                jint dataLen,jlong pts) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    MuxerData *decoder = (MuxerData *) (*env)->GetLongField(env, instance, mp4MuxerField);
    if (decoder == NULL) {
        (*env)->ReleaseByteArrayElements(env, data_, data, 0);
        LOGE("write audio but muxer null");
        return -1;
    }
    int ret = _write_audio_frame_inner_withPts(decoder, data, dataLen, pts);
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
    return ret;
}

bool
_get_extradata_h264(unsigned char *data, unsigned int data_len, unsigned char *extradata,
                    unsigned int *extradata_size) {
    unsigned char *data_buff = data;
    unsigned char sps_nalu[1024] = {0};
    unsigned int sps_len = 0;
    unsigned char pps_nalu[1024] = {0};
    unsigned int pps_len = 0;
    bool issync = 0;
    int index = 0;
    while (JNI_TRUE) {
        if (index + 4 >= data_len) {
            LOGE("nalu find fail %d", data_len);
            return issync;
        }
        if ((*(data_buff + 0) == 0x00 &&
             *(data_buff + 1) == 0x00 &&
             *(data_buff + 2) == 0x00 &&
             *(data_buff + 3) == 0x01 &&
             (((*(data_buff + 4)) & 0x1F) == 0x07)) ||
            *(data_buff + 0) == 0x00 &&
            *(data_buff + 1) == 0x00 &&
            *(data_buff + 2) == 0x01 &&
            (((*(data_buff + 3)) & 0x1F) == 0x07))//SPS
        {
            while (JNI_TRUE) {
                if ((*(data_buff + sps_len + 0) == 0x00 &&
                     *(data_buff + sps_len + 1) == 0x00 &&
                     *(data_buff + sps_len + 2) == 0x00 &&
                     *(data_buff + sps_len + 3) == 0x01 &&
                     (((*(data_buff + sps_len + 4)) & 0x1F) == 0x08)) ||
                    *(data_buff + sps_len + 0) == 0x00 &&
                    *(data_buff + sps_len + 1) == 0x00 &&
                    *(data_buff + sps_len + 2) == 0x01 &&
                    (((*(data_buff + sps_len + 3)) & 0x1F) == 0x08))//PPS
                {
                    memcpy(sps_nalu, data_buff, sps_len);
                    data_buff = data_buff + sps_len;
                    while (JNI_TRUE) {
                        if ((*(data_buff + pps_len + 0) == 0x00 &&
                             *(data_buff + pps_len + 1) == 0x00 &&
                             *(data_buff + pps_len + 2) == 0x00 &&
                             *(data_buff + pps_len + 3) == 0x01 &&
                             (((*(data_buff + pps_len + 4)) & 0x1F) == 0x05)) ||
                            *(data_buff + pps_len + 0) == 0x00 &&
                            *(data_buff + pps_len + 1) == 0x00 &&
                            *(data_buff + pps_len + 2) == 0x01 &&
                            (((*(data_buff + pps_len + 3)) & 0x1F) == 0x05)) {
                            memcpy(pps_nalu, data_buff, pps_len);
                            data_buff = data_buff + pps_len;
                            issync = JNI_TRUE;
                            break;
                        }
                        pps_len++;
                    }
                    break;
                }
                sps_len++;
            }
            break;
        }
        if ((*(data_buff + 0) == 0x00 &&
             *(data_buff + 1) == 0x00 &&
             *(data_buff + 2) == 0x00 &&
             *(data_buff + 3) == 0x01 &&
             (((*(data_buff + 4)) & 0x1F) == 0x01)) ||
            *(data_buff + 0) == 0x00 &&
            *(data_buff + 1) == 0x00 &&
            *(data_buff + 2) == 0x01 &&
            (((*(data_buff + 3)) & 0x1F) == 0x01)) {
            issync = JNI_FALSE;
            break;
        }
        data_buff++;
    }
    *extradata_size = sps_len + pps_len;
    memcpy(extradata, sps_nalu, sps_len);
    memcpy(extradata + sps_len, pps_nalu, pps_len);
    return issync;
}

bool
_get_extradata_h265(unsigned char *data, unsigned int data_len, unsigned char *extradata,
                    unsigned int *extradata_size) {

    unsigned char *data_buff = data;
    unsigned char vps_nalu[1024] = {0};
    unsigned int vps_len = 0;
    unsigned char sps_nalu[1024] = {0};
    unsigned int sps_len = 0;
    unsigned char pps_nalu[1024] = {0};
    unsigned int pps_len = 0;
    bool issync = JNI_FALSE;
    int index = 0;
    while (JNI_TRUE) {
        if (index + 4 >= data_len) {
            LOGE("nalu find fail %d", data_len);
            return issync;
        }
        if ((*(data_buff + 0) == 0x00 &&
             *(data_buff + 1) == 0x00 &&
             *(data_buff + 2) == 0x00 &&
             *(data_buff + 3) == 0x01 &&
             ((((*(data_buff + 4)) & 0x7E) >> 1) == 0x20)) ||
            (*(data_buff + 0) == 0x00 &&
             *(data_buff + 1) == 0x00 &&
             *(data_buff + 2) == 0x01 &&
             ((((*(data_buff + 3)) & 0x7E) >> 1) == 0x20)))//VPS 32
        {
            while (JNI_TRUE) {
                if ((*(data_buff + vps_len + 0) == 0x00 &&
                     *(data_buff + vps_len + 1) == 0x00 &&
                     *(data_buff + vps_len + 2) == 0x00 &&
                     *(data_buff + vps_len + 3) == 0x01 &&
                     ((((*(data_buff + vps_len + 4)) & 0x7E) >> 1) == 0x21)) ||
                    (*(data_buff + vps_len + 0) == 0x00 &&
                     *(data_buff + vps_len + 1) == 0x00 &&
                     *(data_buff + vps_len + 2) == 0x01 &&
                     ((((*(data_buff + vps_len + 3)) & 0x7E) >> 1) ==
                      0x21)))//SPS 33
                {
                    memcpy(vps_nalu, data_buff, vps_len);
                    data_buff = data_buff + vps_len;
                    while (JNI_TRUE) {
                        if ((*(data_buff + sps_len + 0) == 0x00 &&
                             *(data_buff + sps_len + 1) == 0x00 &&
                             *(data_buff + sps_len + 2) == 0x00 &&
                             *(data_buff + sps_len + 3) == 0x01 &&
                             ((((*(data_buff + sps_len + 4)) & 0x7E) >> 1) ==
                              0x22)) ||
                            (*(data_buff + sps_len + 0) == 0x00 &&
                             *(data_buff + sps_len + 1) == 0x00 &&
                             *(data_buff + sps_len + 2) == 0x01 &&
                             ((((*(data_buff + sps_len + 3)) & 0x7E) >> 1) ==
                              0x22)))//PPS 34
                        {
                            memcpy(sps_nalu, data_buff, sps_len);
                            data_buff = data_buff + sps_len;
                            index += sps_len;
                            while (JNI_TRUE) {
                                if ((*(data_buff + pps_len + 0) == 0x00 &&
                                     *(data_buff + pps_len + 1) == 0x00 &&
                                     *(data_buff + pps_len + 2) == 0x00 &&
                                     *(data_buff + pps_len + 3) == 0x01 &&
                                     ((((*(data_buff + pps_len + 4)) & 0x7E)
                                             >> 1) == 0x13)) ||
                                    (*(data_buff + pps_len + 0) == 0x00 &&
                                     *(data_buff + pps_len + 1) == 0x00 &&
                                     *(data_buff + pps_len + 2) == 0x01 &&
                                     ((((*(data_buff + pps_len + 3)) & 0x7E)
                                             >> 1) == 0x13)))//IDR 19
                                {
                                    memcpy(pps_nalu, data_buff, pps_len);
                                    data_buff = data_buff + pps_len;
                                    index += pps_len;
                                    issync = JNI_TRUE;
                                    break;
                                }
                                pps_len++;
                            }
                            break;
                        }
                        sps_len++;
                    }
                    break;
                }
                vps_len++;
            }
            break;
        }
        if ((*(data_buff + 0) == 0x00 &&
             *(data_buff + 1) == 0x00 &&
             *(data_buff + 2) == 0x00 &&
             *(data_buff + 3) == 0x01 &&
             ((((*(data_buff + 4)) & 0x7E) >> 1) == 0x01)) ||
            (*(data_buff + 0) == 0x00 &&
             *(data_buff + 1) == 0x00 &&
             *(data_buff + 2) == 0x01 &&
             ((((*(data_buff + 3)) & 0x7E) >> 1) == 0x01))) {
            issync = JNI_FALSE;
            break;
        }
        index++;
        data_buff++;
    }
    *extradata_size = vps_len + sps_len + pps_len;
    memcpy(extradata, vps_nalu, vps_len);
    memcpy(extradata + vps_len, sps_nalu, sps_len);
    memcpy(extradata + vps_len + sps_len, pps_nalu, pps_len);
    return issync;
}

int _write_video_frame_inner(MuxerData *handle,
                             unsigned char *data,
                             jint data_len,
                             jint duration,
                             jboolean isKeyFrame) {
    bool key = JNI_FALSE;
    int ret = 0;
    AVPacket pkt = {0};
    unsigned char *data_buff_src = data;
    if (handle->_video_stream == NULL) {
        LOGE("write_video_frame video_stream is null");
        return -1;
    }
    if (isKeyFrame) {
        if (handle->_videoCode == video_codec_id_h264) {
            key = _get_extradata_h264(data_buff_src, (unsigned int) data_len,
                                      handle->_video_extradata,
                                      &handle->_video_extradata_size);
        } else {
            key = _get_extradata_h265(data_buff_src, (unsigned int) data_len,
                                      handle->_video_extradata,
                                      &handle->_video_extradata_size);
        }
    }
    if (key == JNI_FALSE && handle->_getfirstframe == JNI_FALSE) {
        LOGE("write_video_frame getfirstframe is not key frame");
        return -1;
    }
    if (handle->_getfirstframe == JNI_FALSE) {
        handle->_video_stream->codec->extradata = handle->_video_extradata;
        handle->_video_stream->codec->extradata_size = handle->_video_extradata_size;
        ret = avformat_write_header(handle->_format_context, NULL);
        if (ret < 0) {
            LOGE("write_video_frame avformat_write_header error = %d", ret);
            avcodec_close(handle->_video_stream->codec);
            handle->_video_stream = NULL;
            return -2;
        }
        handle->_getfirstframe = JNI_TRUE;
    }
    av_init_packet(&pkt);
    if (key == JNI_TRUE) {
        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.data = data_buff_src + handle->_video_extradata_size;
        pkt.size = data_len - handle->_video_extradata_size;
    } else {
        pkt.data = data_buff_src;
        pkt.size = data_len;
    }
    handle->_video_frame_index++;
//    pkt.pts = av_rescale(handle->_video_frame_index++,
//                         handle->_video_stream->time_base.den,
//                         handle->_video_stream->codec->time_base.den);
    pkt.stream_index = handle->_video_stream->index;
    if (duration == 0) {
        pkt.duration = 90000 / handle->_fps;
    } else {
        pkt.duration = duration * 90;
    }
    handle->_video_time += pkt.duration;
    pkt.pts = handle->_video_time;
    pkt.dts = pkt.pts;
    pkt.pos = -1;
    return av_interleaved_write_frame(handle->_format_context, &pkt);
}

JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_writeVideo(JNIEnv *env, jobject instance,
                                                                jbyteArray data_,
                                                                jint dataLen, jint durtion,
                                                                jboolean iskeyFrame) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    MuxerData *decoder = (MuxerData *) (*env)->GetLongField(env, instance, mp4MuxerField);
    if (decoder == NULL) {
        (*env)->ReleaseByteArrayElements(env, data_, data, 0);
        LOGE("write video but muxer null");
        return -1;
    }
    int ret = _write_video_frame_inner(decoder, (unsigned char *) data, dataLen, durtion, iskeyFrame);
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
    return ret;
}

int _write_video_frame_inner_pts(MuxerData *handle,
                             unsigned char *data,
                             jint data_len,
                             jlong pts, jboolean isKeyFrame) {
    bool key = JNI_FALSE;
    int ret = 0;
    AVPacket pkt = {0};
    unsigned char *data_buff_src = data;
    if (handle->_video_stream == NULL) {
        LOGE("write_video_frame video_stream is null");
        return -1;
    }
    if (isKeyFrame) {
        if (handle->_videoCode == video_codec_id_h264) {
            key = _get_extradata_h264(data_buff_src, (unsigned int) data_len,
                                      handle->_video_extradata,
                                      &handle->_video_extradata_size);
        } else {
            key = _get_extradata_h265(data_buff_src, (unsigned int) data_len,
                                      handle->_video_extradata,
                                      &handle->_video_extradata_size);
        }
    }
    if (key == JNI_FALSE && handle->_getfirstframe == JNI_FALSE) {
        LOGE("write_video_frame getfirstframe is not key frame");
        return -1;
    }
    if (handle->_getfirstframe == JNI_FALSE) {
        handle->_video_stream->codec->extradata = handle->_video_extradata;
        handle->_video_stream->codec->extradata_size = handle->_video_extradata_size;
        ret = avformat_write_header(handle->_format_context, NULL);
        if (ret < 0) {
            LOGE("write_video_frame avformat_write_header error = %d", ret);
            avcodec_close(handle->_video_stream->codec);
            handle->_video_stream = NULL;
            return -2;
        }
        handle->_getfirstframe = JNI_TRUE;
    }
    av_init_packet(&pkt);
    if (key == JNI_TRUE) {
        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.data = data_buff_src + handle->_video_extradata_size;
        pkt.size = data_len - handle->_video_extradata_size;
    } else {
        pkt.data = data_buff_src;
        pkt.size = data_len;
    }
    pkt.stream_index = handle->_video_stream->index;
    pkt.pts = pts;
    pkt.dts = pkt.pts;
    return av_interleaved_write_frame(handle->_format_context, &pkt);
}

JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_writeVideoWithPts(JNIEnv *env, jobject instance, jbyteArray data_,
                                            jint dataLen, jlong pts, jboolean iskeyFrame) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    MuxerData *decoder = (MuxerData *) (*env)->GetLongField(env, instance, mp4MuxerField);
    if (decoder == NULL) {
        (*env)->ReleaseByteArrayElements(env, data_, data, 0);
        LOGE("write video but muxer null");
        return -1;
    }
    int ret = _write_video_frame_inner_pts(decoder, (unsigned char *) data, dataLen, pts, iskeyFrame);
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
    return ret;
}

int _close_file_for_create_inner(MuxerData *handle) {
    if (handle->_format_context == NULL) {
        LOGE("_close_file_for_create_inner");
        return -1;
    }
    int ret = 0;
    if (handle->_getfirstframe == JNI_TRUE) {
        ret = av_write_trailer(handle->_format_context);
        if (ret != 0) {
            LOGE("av_write_trailer error = %d", ret);
        }
    }
    if (handle->_video_stream != NULL) {
        avcodec_close(handle->_video_stream->codec);
        handle->_video_stream->codec->extradata = NULL;
        handle->_video_stream->codec->extradata_size = 0;
        handle->_video_stream = NULL;
    }
    if (handle->_audio_stream != NULL) {
        avcodec_close(handle->_audio_stream->codec);
        handle->_audio_stream->codec->extradata = NULL;
        handle->_audio_stream->codec->extradata_size = 0;
        handle->_audio_stream = NULL;
    }
    if(handle->_format_context != NULL) {
        int temp = avio_close(handle->_format_context->pb);
        if (temp != 0) {
            LOGE("avio_close error = %d", temp);
            ret = temp;
        }
        handle->_format_context->pb = NULL;
    }
    if(handle->_format_context != NULL) {
        avformat_free_context(handle->_format_context);
        handle->_format_context = NULL;
    }
    handle->_audio_frame_index = 0;
    if (handle->_audio_extradata != NULL) {
        free(handle->_audio_extradata);
        handle->_audio_extradata = NULL;
    }
    handle->_audio_extradata_size = 0;
    
    handle->_video_frame_index = 0;
    handle->_video_time = 0;
    if(handle->_video_extradata != NULL){
        free(handle->_video_extradata);
        handle->_video_extradata = NULL;
    }
    handle->_video_extradata_size = 0;
    handle->_getfirstframe = JNI_FALSE;
    LOGE("close_file_for_create success");
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_xiaomi_smarthome_fastvideo_decoder_Mp4Muxer_closeFile(JNIEnv *env, jobject instance) {
    MuxerData *decoder = (MuxerData *) (*env)->GetLongField(env, instance, mp4MuxerField);
    if (decoder == NULL) {
        LOGE("save mp4 but muxer null");
        return -1;
    }
    int ret = _close_file_for_create_inner(decoder);
    if(decoder != NULL) {
        free(decoder);
    }
    decoder = NULL;
    return ret;
}