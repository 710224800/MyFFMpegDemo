#include <jni.h>
#include <string>
extern "C"{
#include <libavcodec/avcodec.h>
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lyhao_xplay_NativeLib_getFFMpegConfig(JNIEnv *env, jobject thiz) {
    std::string hello = "avcodec_configuration() = \n";
    hello += avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}