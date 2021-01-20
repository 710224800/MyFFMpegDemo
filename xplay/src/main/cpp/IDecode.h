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
};


#endif //XPLAY_IDECODE_H
