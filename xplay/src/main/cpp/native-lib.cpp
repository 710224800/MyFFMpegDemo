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

IDemux *de = nullptr;
IDecode *vdecode = nullptr;
IDecode *adecode = nullptr;
IVideoView *view = nullptr;
IResample *resample = nullptr;

class TestObserver : public IObserver{
public:
    void update(XData d){
        XLOGI("TestObs Update data size is %d", d.size);
    }
};
TestObserver *testObs = nullptr;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lyhao_xplay_NativeLib_testIDemuxOpen(JNIEnv *env, jobject thiz, jstring url_) {
    std::string hello = "hello world c++";
    const char *url = env->GetStringUTFChars(url_, nullptr);
    if(de == nullptr){
        de = new FFDemux();
    }
    de->open(url);
    vdecode = new FFDecode();
    vdecode->open(de->getVPara());
    de->addObs(vdecode);

    adecode = new FFDecode();
    adecode->open(de->getAPara());
    de->addObs(adecode);

    view = new GLVideoView();
    vdecode->addObs(view);

    resample = new FFResample();
    resample->open(de->getAPara());
    adecode->addObs(resample);

    vdecode->start();
    adecode->start();
    de->start();
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