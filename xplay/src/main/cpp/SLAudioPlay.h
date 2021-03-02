//
// Created by lyhao on 21-3-2.
//

#ifndef MYFFMPEGDEMO_SLAUDIOPLAY_H
#define MYFFMPEGDEMO_SLAUDIOPLAY_H


#include "IAudioPlay.h"

class SLAudioPlay : public IAudioPlay{
public:
    virtual bool startPlay(XParameter out);
    void playCall(void *bufq);

};


#endif //MYFFMPEGDEMO_SLAUDIOPLAY_H
