//
// Created by lyhao on 21-3-2.
//

#ifndef MYFFMPEGDEMO_FFRESAMPLE_H
#define MYFFMPEGDEMO_FFRESAMPLE_H


#include "IResample.h"
struct SwrContext;
class FFResample : public IResample{
public:
    virtual bool open(XParameter in, XParameter out = XParameter());
    virtual XData resample(XData indata);

protected:
    SwrContext *actx = nullptr;
};


#endif //MYFFMPEGDEMO_FFRESAMPLE_H
