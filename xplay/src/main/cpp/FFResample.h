//
// Created by lyhao on 21-3-2.
//

#ifndef XPLAY_FFRESAMPLE_H
#define XPLAY_FFRESAMPLE_H


#include "IResample.h"
struct SwrContext;
class FFResample : public IResample{
public:
    virtual bool open(XParameter in, XParameter out = XParameter());
    virtual XData resample(XData indata);

protected:
    SwrContext *actx = nullptr;
};


#endif //XPLAY_FFRESAMPLE_H
