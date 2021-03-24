//
// Created by lyhao on 21-3-2.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H


#include <list>
#include "XData.h"
#include "XParameter.h"
#include "IObserver.h"

class IAudioPlay : public IObserver{
public:
    virtual void update(XData data);

    //获取缓冲数据，如没有则阻塞
    virtual XData getData();

    virtual bool startPlay(XParameter out) = 0;
    virtual void close() = 0;
    virtual void clear();

    //最大缓冲
    int maxFrame = 100;
    int pts = 0;
protected:
    std::list<XData> frames;
    std::mutex framesMutex;
};


#endif //XPLAY_IAUDIOPLAY_H
