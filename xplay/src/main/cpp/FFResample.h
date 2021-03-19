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
    virtual void close();
    virtual XData resample(XData indata);

protected:
    SwrContext *actx = nullptr;
    std::mutex ffresample_mux;
};


#endif //XPLAY_FFRESAMPLE_H
