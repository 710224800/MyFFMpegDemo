//
// Created by lyhao on 20-9-21.
//
#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"ywl5320",FORMAT,##__VA_ARGS__);

static SLObjectItf slObjItf = nullptr;
SLEngineItf CreateSL(){
    SLresult re;
    SLEngineItf en;
    re = slCreateEngine(&slObjItf, 0, 0, 0, 0, 0);
    if(re != SL_RESULT_SUCCESS) return nullptr;

    re = (*slObjItf)->Realize(slObjItf, SL_BOOLEAN_FALSE);
    if(re != SL_RESULT_SUCCESS) return nullptr;

    re = (*slObjItf)->GetInterface(slObjItf, SL_IID_ENGINE, &en);
    if(re != SL_RESULT_SUCCESS) return nullptr;
    return en;
}

static const char* pcmurl = nullptr;
static jstring pcmurl_ = nullptr;
void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *context){
    LOGE("PcmCall pcmurl= % s", pcmurl);
    static FILE *fp = nullptr;
    static char *buf = nullptr;
    if(buf == nullptr){
        buf = new char[1024 * 1024];
    }
    if(fp == nullptr){
        fp = fopen(pcmurl, "rb");
    }
    if(!feof(fp)){
        int len = fread(buf, 1, 1024, fp);
        if(len > 0){
            (*bf)->Enqueue(bf, buf, len);
        }
    } else {
        LOGE("end ReleaseStringUTFChars"); // 这块代码还是有问题，不太明白为什么没有调用到
        ((JNIEnv *)context)->ReleaseStringUTFChars(pcmurl_, pcmurl);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_myffdemo_NativeLib_openslTest(JNIEnv *env, jobject thiz, jstring url_,
                                               jobject handle) {
    pcmurl_ = url_;
    pcmurl = env->GetStringUTFChars(url_, nullptr);
    //1.创建引擎
    SLEngineItf slEngineItf = CreateSL();
    if(slEngineItf == nullptr){
        LOGE("CreateSL failed");
        return -1;
    }
    LOGE("CreateSL success");
    //2.创建混间器
    SLObjectItf mix = nullptr;
    SLresult re = 0;
    re = (*slEngineItf)->CreateOutputMix(slEngineItf, &mix, 0, nullptr, nullptr);
    if(re != SL_RESULT_SUCCESS){
        LOGE("CreateOutputMix failed");
        return -1;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if(re != SL_RESULT_SUCCESS){
        LOGE("Realize failed");
        return -1;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};
    LOGE("audioSink success");

    //3.配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM, 2, // 声道数
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN // 字节序，小端
    };
    SLDataSource dts = {&queue, &pcm};

    //4.创建播放器
    SLObjectItf player = nullptr;
    SLPlayItf iplayer = nullptr;
    SLAndroidSimpleBufferQueueItf pcmQue = nullptr;
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*slEngineItf)->CreateAudioPlayer(slEngineItf, &player, &dts, &audioSink,
                                      sizeof(ids)/ sizeof(SLInterfaceID), ids, req);
    if(re != SL_RESULT_SUCCESS){
        LOGE("Realize failed");
        return -1;
    }
    (*player) -> Realize(player, SL_BOOLEAN_FALSE);

    //获取player接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if(re !=SL_RESULT_SUCCESS )
    {
        LOGE("GetInterface SL_IID_PLAY failed!");
        return -1;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if(re !=SL_RESULT_SUCCESS )
    {
        LOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return -1;
    }
    //设置回调函数，播放队列空调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, env);

    //设置为播放状态
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    url_ = nullptr;
    return 0;
}