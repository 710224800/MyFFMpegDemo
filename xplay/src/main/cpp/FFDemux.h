//
// Created by lyhao on 20-10-10.
//

#ifndef XPLAY_FFDEMUX_H
#define XPLAY_FFDEMUX_H

#include "IDemux.h"
struct AVFormatContext;

class FFDemux : public IDemux {
public:
    //打开文件，或者流媒体
    virtual bool open(const char *url);
    // seek pos 0.0 - 1.0
    virtual bool seek(double pos);
    virtual void close();

    //获取视频参数
    virtual XParameter getVPara();

    //获取音频参数
    virtual XParameter getAPara();

    //读取一帧数据，数据由调用者清理
    virtual XData read();

    FFDemux();

private:
    AVFormatContext *avFormatContext = nullptr;
    std::mutex ffdemux_mux;
    int audioStream = 1;
    int videoStream = 0;
};


#endif //XPLAY_FFDEMUX_H
