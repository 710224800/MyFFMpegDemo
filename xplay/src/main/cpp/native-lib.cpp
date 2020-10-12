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

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lyhao_xplay_NativeLib_testIDemuxOpen(JNIEnv *env, jobject thiz, jstring url_) {
    std::string hello = "hello world c++";
    const char *url = env->GetStringUTFChars(url_, nullptr);

    IDemux *de = new FFDemux();
    de->Open(url);
    de->Start();
    env->ReleaseStringUTFChars(url_, url);
    return env->NewStringUTF(hello.c_str());
}