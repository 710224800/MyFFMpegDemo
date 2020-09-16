#include <jni.h>
#include <string>
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "native-lib", __VA_ARGS__)
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
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

double r2d(AVRational avRational){
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
    //关闭
    avformat_close_input(&avFormatContext);
    //释放String内存
    env->ReleaseStringUTFChars(url_, url);
    return 0;
}

