//
// Created by lyhao on 20-10-10.
//

#ifndef XPLAY_IDEMUX_H
#define XPLAY_IDEMUX_H

#include "XData.h"
#include "IObserver.h"
#include "XParameter.h"

//解封装接口
class IDemux : public IObserver{
public:
    //打开文件，或者流媒体 rtmp http rtsp
    virtual bool open(const char *url) = 0;  //纯虚函数

    //获取视频参数
    virtual XParameter getVPara() = 0;

    //获取音频参数
    virtual XParameter getAPara() = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData read() = 0; //纯虚函数

    //总时长（毫秒）
    unsigned long totalMs = 0;
protected:
    virtual void main();
};


#endif //XPLAY_IDEMUX_H
