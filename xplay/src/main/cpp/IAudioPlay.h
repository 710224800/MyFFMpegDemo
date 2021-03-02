//
// Created by lyhao on 21-3-2.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H


#include "XData.h"
#include "XParameter.h"
#include "IObserver.h"

class IAudioPlay : public IObserver{
public:
    virtual void update(XData data);
    virtual bool startPlay(XParameter out) = 0;
};


#endif //XPLAY_IAUDIOPLAY_H
