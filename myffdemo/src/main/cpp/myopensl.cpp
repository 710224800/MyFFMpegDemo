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

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_myffdemo_NativeLib_openslTest(JNIEnv *env, jobject thiz, jstring url,
                                               jobject handle) {
    SLEngineItf slEngineItf = CreateSL();
    if(slEngineItf == nullptr){
        LOGE("CreateSL failed");
    }
    LOGE("CreateSL success");
    return 0;
}