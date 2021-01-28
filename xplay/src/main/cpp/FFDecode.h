//
// Created by lyhao on 21-1-20.
//

#ifndef XPLAY_FFDECODE_H
#define XPLAY_FFDECODE_H


#include "IDecode.h"

struct AVCodecContext;
struct AVFrame;

class FFDecode : public IDecode{
public:
    virtual bool open(XParameter para);

    //future 模型，发送数据到线程解码
    virtual bool sendPacket(XData pkt);

    //从线程中读取解码结果
    virtual XData recvFrame();

protected:
    AVCodecContext *avCodecContext = nullptr;
    AVFrame *avFrame = nullptr;
};


#endif //XPLAY_FFDECODE_H
