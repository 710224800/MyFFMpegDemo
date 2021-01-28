//
// Created by lyhao on 21-1-20.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H


#include "IObserver.h"
#include "XParameter.h"

class IDecode : public IObserver{
public:
    // 打开解码器
    virtual bool open(XParameter parm) = 0;

    //future 模型，发送数据到线程解码
    virtual bool sendPacket(XData pkt) = 0;

    //从线程中读取解码结果
    virtual XData recvFrame() = 0;
};


#endif //XPLAY_IDECODE_H
