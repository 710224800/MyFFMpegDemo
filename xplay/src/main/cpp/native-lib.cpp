#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "XLog.h"
extern "C"{
#include <libavcodec/avcodec.h>
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lyhao_xplay_NativeLib_getFFMpegConfig(JNIEnv *env, jobject thiz) {
    std::string hello = "avcodec_configuration() = \n";
    hello += avcodec_configuration();
    const char * c_str = hello.c_str();
    XLOGD("getFFMpegConfig %s", c_str);
    return env->NewStringUTF(c_str);
}

#include "FFDemux.h"
#include "FFDecode.h"
#include "XEGL.h"
#include "XShader.h"
#include "IVideoView.h"
#include "GLVideoView.h"
#include "IResample.h"
#include "FFResample.h"
#include "IAudioPlay.h"
#include "SLAudioPlay.h"
#include "IPlayer.h"

IDemux *de = nullptr;
IDecode *vdecode = nullptr;
IDecode *adecode = nullptr;
IVideoView *view = nullptr;
IResample *resample = nullptr;
IAudioPlay *audioPlay = nullptr;

class TestObserver : public IObserver{
public:
    void update(XData d){
        XLOGI("TestObs Update data size is %d", d.size);
    }
};
TestObserver *testObs = nullptr;

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *res){
    FFDecode::initHard(vm);
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lyhao_xplay_NativeLib_testIDemuxOpen(JNIEnv *env, jobject thiz, jstring url_) {
    std::string hello = "hello world c++";
    const char *url = env->GetStringUTFChars(url_, nullptr);
    if(de == nullptr){
        de = new FFDemux();
    }
//    de->open(url); // 解封装器

    vdecode = new FFDecode();
//    vdecode->open(de->getVPara(), true); // 用硬件解码可以了
    de->addObs(vdecode); // 为解封装器添加 视频解码器

    adecode = new FFDecode();
//    adecode->open(de->getAPara());
    de->addObs(adecode); // 为解封装器添加 音频解码器

    view = new GLVideoView();
    vdecode->addObs(view); // 为视频解码器 添加显示窗口，openGl实现

    resample = new FFResample();
//    XParameter outPara = de->getAPara();
//    resample->open(de->getAPara(), outPara);
    adecode->addObs(resample); // 为音频解码器 添加重采样器，先这么叫

    audioPlay = new SLAudioPlay();
//    audioPlay->startPlay(outPara);
    resample->addObs(audioPlay);

//    vdecode->start();
//    adecode->start();
//    de->start();
    IPlayer::get()->demux = de;
    IPlayer::get()->adecode = adecode;
    IPlayer::get()->vdecode = vdecode;
    IPlayer::get()->videoView = view;
    IPlayer::get()->resample = resample;
    IPlayer::get()->audioPlay = audioPlay;

    IPlayer::get()->open(url);
//    XSleep(3000); // 这个方法会阻塞主线程
//    de->stop();
    env->ReleaseStringUTFChars(url_, url);
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyhao_xplay_NativeLib_testStop(JNIEnv *env, jobject thiz) {
    if(de != nullptr){
        de->stop();
        delete de;
        de = nullptr;
    }
    if(adecode != nullptr){
        adecode->stop();
        delete adecode;
        adecode = nullptr;
    }
    if(vdecode != nullptr){
        vdecode->stop();
        delete vdecode;
        vdecode = nullptr;
    }
    if(view != nullptr){
        delete view;
        view = nullptr;
    }
    if(resample != nullptr){
        delete resample;
        resample = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyhao_xplay_NativeLib_initView(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement initView()
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    view->setRender(win);
//    XEGL::get() -> init(win);
//    XShader shader;
//    shader.init();
}