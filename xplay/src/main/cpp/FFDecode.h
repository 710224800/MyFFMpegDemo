//
// Created by lyhao on 21-1-20.
//

#ifndef XPLAY_FFDECODE_H
#define XPLAY_FFDECODE_H


#include "IDecode.h"

struct AVCodecContext;

class FFDecode : public IDecode{
public:
    virtual bool open(XParameter para);

protected:
    AVCodecContext *codecContext = 0;
};


#endif //XPLAY_FFDECODE_H
