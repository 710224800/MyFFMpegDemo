//
// Created by lyhao on 21-3-11.
//

#ifndef XPLAY_IPLAYERBUILDER_H
#define XPLAY_IPLAYERBUILDER_H


#include "IPlayer.h"

class IPlayerBuilder {
public:
    virtual IPlayer *buildPlayer(unsigned char index = 0);

protected:
    virtual IDemux *createDemux() = 0;
    virtual IDecode *createDecode() = 0;
    virtual IResample *createResample() = 0;
    virtual IVideoView *createVideoView() = 0;
    virtual IAudioPlay *createAudioPlay() = 0;
    virtual IPlayer *createPlayer(unsigned char index = 0) = 0;
};


#endif //XPLAY_IPLAYERBUILDER_H
