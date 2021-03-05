//
// Created by lyhao on 21-1-20.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H


#include "IObserver.h"
#include "XParameter.h"
#include <list>

class IDecode : public IObserver{
public:
    // 打开解码器
    virtual bool open(XParameter parm, bool isHard = false) = 0;

    //future 模型，发送数据到线程解码
    virtual bool sendPacket(XData pkt) = 0;

    //从线程中读取解码结果，再次调用会复用上次空间，线程不安全
    virtual XData recvFrame() = 0;

    //由主体notify的数据，阻塞
    virtual void update(XData pkt);

    bool isAudio = false;

    //最大队列缓冲
    int maxList = 100;
protected:
    virtual void main();

    std::list<XData> packs;
    std::mutex packsMutex;
};


#endif //XPLAY_IDECODE_H
