//
// Created by lyhao on 20-10-10.
//

#ifndef XPLAY_XLOG_H
#define XPLAY_XLOG_H

class XLog {

};
#ifdef ANDROID
#include <android/log.h>
#define XLOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"CPlay",__VA_ARGS__)
#define XLOGI(...) __android_log_print(ANDROID_LOG_INFO,"CPlay",__VA_ARGS__)
#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR,"CPlay",__VA_ARGS__)
#else
#define XLOGD(...) printf("CPlay",__VA_ARGS__)
#define XLOGI(...) printf("CPlay",__VA_ARGS__)
#define XLOGE(...) printf("CPlay",__VA_ARGS__)

#endif


#endif //XPLAY_XLOG_H
