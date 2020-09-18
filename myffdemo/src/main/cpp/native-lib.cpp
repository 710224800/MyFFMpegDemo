#include <jni.h>
#include <string>
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "native-lib", __VA_ARGS__)
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
#include <libswscale/swscale.h>
}

extern "C" JNIEXPORT jint
JNI_OnLoad(JavaVM *vm, void *res){
    av_jni_set_java_vm(vm, 0); // 为ffmpeg设置 java vm

    ///////////// 下面这是一段学习JNI_OnLoad的测试代码
    JNIEnv *env = NULL;
    if(vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK){
        LOGE("无法通过vm获取JNIEnv");
    }
    LOGE("通过vm获取JNIEnv success");
    ///////////// 测试代码结束

    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myffdemo_NativeLib_getFFMpegConfiguration(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "avcodec_configuration() = \n";
    hello += avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}
/**
 * 测试fopen 打开文件
 */
extern "C" JNIEXPORT jint JNICALL
Java_com_example_myffdemo_NativeLib_testFileOpen(
        JNIEnv* env, jobject instance, jstring url_, jobject handle) {
    const char *url = env->GetStringUTFChars(url_, nullptr);
    LOGE("receive url= %s", url);
    FILE *file = fopen(url, "rb");
    if(!file){
        LOGE("open %s failed! %d reason=%s", url, errno, strerror(errno));
    } else {
        LOGE("open %s success!", url);
        fclose(file);
    }
    env->ReleaseStringUTFChars(url_, url);
    return 0;
}

static double r2d(AVRational avRational){
    double result = avRational.num == 0 || avRational.den == 0 ? 0 : (double)avRational.num / (double)avRational.den;
    LOGE("r2d result=%f", result);
    return result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_myffdemo_NativeLib_avformatOpenInput(
        JNIEnv* env, jobject instance, jstring url_, jobject handle) {
    const char *url = env->GetStringUTFChars(url_, nullptr);
    LOGE("avformatOpenInput receive url= %s", url);
    //初始化解封装
    av_register_all();
    //初始化网络
    avformat_network_init();

    avcodec_register_all();//这个貌似不调用也没关系，有待考证（应该是在某个新版，废弃了，不再需要调用）

    //打开文件
    AVFormatContext *avFormatContext = NULL;
    int re = avformat_open_input(&avFormatContext, url, 0, 0);
    if(re != 0){
        LOGE("avformat_open_input %s failed reason=%s", url, av_err2str(re));
        return -1;
    }
    LOGE("avformat_open_input %s success", url);
    re = avformat_find_stream_info(avFormatContext, 0);
    if(re != 0){
        LOGE("avformat_find_stream_info failed !");
        return -1;
    }
    LOGE("avformat_find_stream_info duration=%lld, nb_streams=%d",avFormatContext->duration,
            avFormatContext->nb_streams);

    double fps = 0;
    int videoStream = 0;
    int audioStream = 1;
    for(int i = 0; i < avFormatContext->nb_streams; i++){
        AVStream *avStream = avFormatContext->streams[i];
        if(avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = i;
            LOGE("视频数据 = %d", videoStream);
            fps = r2d(avStream->avg_frame_rate);
            LOGE("fps=%f, width=%d, height=%d, codecid=%d, pixformat=%d", fps,
                    avStream->codecpar->width, avStream->codecpar->height,
                    avStream->codecpar->codec_id, avStream->codecpar->format);
        } else if(avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioStream = i;
            LOGE("音频数据 = %d", audioStream);
            LOGE("sample_rate=%d, channels=%d, sample_format=%d", avStream->codecpar->sample_rate,
                    avStream->codecpar->channels, avStream->codecpar->format);
        }
    }
    //获取stream id的另一种方式：av_find_best_stream
    audioStream = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    LOGE("av_find_best_stream audioStream=%d", audioStream);//这里是看一下audioStream id是多少，用于下面区分

    //获取视频解码器（软解码器）
    AVCodec *vdecoder = avcodec_find_decoder(avFormatContext->streams[videoStream]->codecpar->codec_id);
    //硬解码
    vdecoder = avcodec_find_decoder_by_name("h264_mediacodec");
    if(vdecoder == nullptr){ // !decoder
        LOGE("avcodec_find_decoder vdecoder failed");
        return -1;
    }
    LOGE("avcodec_find_decoder vdecoder success");
    //解码器初始化
    AVCodecContext *vcc = avcodec_alloc_context3(vdecoder);
    avcodec_parameters_to_context(vcc, avFormatContext->streams[videoStream]->codecpar);
    vcc->thread_count = 1;
    //打开解码器
    re = avcodec_open2(vcc, 0, 0);
    if(re != 0){
        LOGE("avcodec_open2 vdecoder failed");
        return -1;
    }
    ////////////////////////////////////////////////////////////////////
    //打开音频解码器
    AVCodec * adecoder = avcodec_find_decoder(avFormatContext->streams[audioStream]->codecpar->codec_id);
    if(!adecoder){
        LOGE("avcodec_find_decoder adecoder failed");
        return -1;
    }
    LOGE("avcodec_find_decoder adecoder success");
    AVCodecContext *acc = avcodec_alloc_context3(adecoder);
    avcodec_parameters_to_context(acc, avFormatContext->streams[audioStream]->codecpar);
    acc->thread_count = 1;

    re = avcodec_open2(acc, 0, 0);
    if(re != 0){
        LOGE("avcodec_open2 adecoder failed");
        return -1;
    }

    //读取数据
    AVPacket *pkt = av_packet_alloc();
    AVFrame *avFrame = av_frame_alloc();
    //相素转换
    SwsContext *swsContext = NULL;
    int outWidth = 1280;
    int outHeight = 720;
    char *rgb = new char[1920 * 1080 * 4];

    while(true){
        re = av_read_frame(avFormatContext, pkt);
        if(re != 0){
            LOGE("读取到结尾处");
            AVRational timeBase = avFormatContext->streams[videoStream]->time_base;
            double timeBaseDouble = r2d(timeBase);
            LOGE("videoStream timeBase = %d / %d  double=%f", timeBase.num, timeBase.den, timeBaseDouble);
            int pos = (int)(20 * timeBaseDouble);
            LOGE("seek to pos=%d", pos);
            av_seek_frame(avFormatContext, videoStream, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
            continue; // 这里seek后，继续读取
        }
        LOGE("stream=%d, size=%d, pts=%lld, flags=%d", pkt->stream_index, pkt->size,
             pkt->pts, pkt->flags);
        AVCodecContext *cc = vcc;
        if(pkt->stream_index == audioStream){
            cc = acc;
        }
        //发送到线程中解码
        re = avcodec_send_packet(cc, pkt);
        //清理
        int p = pkt->pts;
        av_packet_unref(pkt); // 引用计数-1 // 释放内存，要不然内存会一直增长

        //读陬Frame
        if(re != 0){
            LOGE("avcodec_send_packet failed");
        } else {
            for(;;){
                re = avcodec_receive_frame(cc, avFrame);
                if(re != 0){
                    //LOGE("avcodec_receive_frame failed");
                    break;
                }
                //LOGE("avcodec_receive_frame pts=%lld", avFrame->pts);
                if(cc == vcc){
                    swsContext = sws_getCachedContext(swsContext,
                            avFrame->width, avFrame->height,(AVPixelFormat)avFrame->format,
                            outWidth, outHeight, AV_PIX_FMT_RGBA, SWS_FAST_BILINEAR,
                            0, 0, 0);
                    if(swsContext == nullptr){
                        LOGE("sws_getCachedContext failed");
                    } else {
                        uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
                        data[0] = (uint8_t *) rgb;
                        int lines[AV_NUM_DATA_POINTERS] = {0};
                        lines[0] = outWidth * 4;

                        int height = sws_scale(swsContext, (const uint8_t **)avFrame->data, avFrame->linesize,
                                0, avFrame->height, data, lines);
                        LOGE("sws_scale = %d", height);
                    }
                }
            }
        }

        av_usleep(500000); // 延时1秒
    }
    //关闭
    avformat_close_input(&avFormatContext);
    //释放String内存
    env->ReleaseStringUTFChars(url_, url);
    return 0;
}

