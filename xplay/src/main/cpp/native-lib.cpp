#include <jni.h>
#include <string>
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
    if(testObs == nullptr){
        testObs = new TestObserver();
    }
    vdecode = new FFDecode();
//    vdecode->open()
    de->addObs(testObs);
    de->Open(url);
    de->Start();
//    XSleep(3000); // 这个方法会阻塞主线程
//    de->Stop();
    env->ReleaseStringUTFChars(url_, url);
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyhao_xplay_NativeLib_testStop(JNIEnv *env, jobject thiz) {
    if(de != nullptr){
        de->Stop();
    }
}