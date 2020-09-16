#include <jni.h>
#include <string>
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "native-lib", __VA_ARGS__)
extern "C"{
#include <libavcodec/avcodec.h>
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myffdemo_NativeLib_getFFMpegConfiguration(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "avcodec_configuration() = \n";
    hello += avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

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
