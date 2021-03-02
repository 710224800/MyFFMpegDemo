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
    virtual bool startPlay(XParameter out) = 0;

    int maxFrame = 100;
protected:
    std::list<XData> frames;
    std::mutex framesMutex;
};


#endif //XPLAY_IAUDIOPLAY_H
