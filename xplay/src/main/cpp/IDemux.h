//
// Created by lyhao on 20-10-10.
//

#ifndef XPLAY_IDEMUX_H
#define XPLAY_IDEMUX_H

#include <time64.h>
#include "XData.h"
//解封装接口
class IDemux {
public:
    //打开文件，或者流媒体 rtmp http rtsp
    virtual bool Open(const char *url) = 0;  //纯虚函数

    //读取一帧数据，数据由调用者清理
    virtual XData Read() = 0; //纯虚函数

    //总时长（毫秒）
    int64_t totalMs = 0;
};


#endif //XPLAY_IDEMUX_H
