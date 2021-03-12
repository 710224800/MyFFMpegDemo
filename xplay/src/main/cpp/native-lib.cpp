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

#include "IPlayer.h"
#include "FFPlayerBuilder.h"
#include "IPlayerPorxy.h"

IPlayer *player = nullptr;

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *res){
//    FFDecode::initHard(vm);
//    FFPlayerBuilder::initHard(vm);
    IPlayerPorxy::get()->init(vm);
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lyhao_xplay_NativeLib_testIDemuxOpen(JNIEnv *env, jobject thiz, jstring url_) {
    std::string hello = "hello world c++";
    const char *url = env->GetStringUTFChars(url_, nullptr);
//    player = FFPlayerBuilder::get()->buildPlayer();
//    player->open(url);
//    player->startPlay();
    IPlayerPorxy::get()->open(url);
    IPlayerPorxy::get()->startPlay();

    env->ReleaseStringUTFChars(url_, url);
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyhao_xplay_NativeLib_testStop(JNIEnv *env, jobject thiz) {
    IPlayerPorxy::get()->release();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyhao_xplay_NativeLib_initView(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement initView()
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
//    if(player != nullptr) {
//        player->initView(win);
//    }
    IPlayerPorxy::get()->initView(win);
//    XEGL::get() -> init(win);
//    XShader shader;
//    shader.init();
}