//
// Created by lyhao on 21-3-2.
//

#ifndef XPLAY_IRESAMPLE_H
#define XPLAY_IRESAMPLE_H


#include "IObserver.h"
#include "XParameter.h"

class IResample : public IObserver{
public:
    virtual bool open(XParameter in, XParameter out = XParameter()) = 0;
    virtual XData resample(XData indata) = 0;
    virtual void update(XData data);
    int outChannels = 2;
    int outFormat = 1;
};


#endif //XPLAY_IRESAMPLE_H
