//
// Created by lyhao on 21-3-9.
//

#ifndef XPLAY_IPLAYER_H
#define XPLAY_IPLAYER_H


#include "XThread.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IResample.h"
#include "IVideoView.h"
#include "IAudioPlay.h"

class IPlayer : public XThread{
public:
    static IPlayer *get(unsigned char index = 0);
    virtual bool open(const char *path);
    virtual bool startPlay();

    bool isHardDecode = true;

    IDemux *demux = nullptr;
    IDecode *vdecode = nullptr;
    IDecode *adecode = nullptr;
    IResample *resample = nullptr;
    IVideoView *videoView = nullptr;
    IAudioPlay *audioPlay = nullptr;

protected:
    IPlayer(){};
};


#endif //XPLAY_IPLAYER_H
