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

IDemux *de = nullptr;
IDecode *vdecode = nullptr;
IDecode *adecode = nullptr;

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
        de = nullptr;
    }
    if(adecode != nullptr){
        adecode->stop();
        adecode = nullptr;
    }
    if(vdecode != nullptr){
        vdecode->stop();
        vdecode = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyhao_xplay_NativeLib_initView(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement initView()
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
}