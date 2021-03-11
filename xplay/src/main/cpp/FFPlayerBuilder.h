//
// Created by lyhao on 21-3-11.
//

#ifndef XPLAY_FFPLAYERBUILDER_H
#define XPLAY_FFPLAYERBUILDER_H


#include "IPlayerBuilder.h"

class FFPlayerBuilder : public IPlayerBuilder{
public:
    static void initHard(void *vm);
    static FFPlayerBuilder *get(){
        static FFPlayerBuilder ff;
        return &ff;
    }

protected:
    FFPlayerBuilder(){};
    virtual IDemux *createDemux();
    virtual IDecode *createDecode();
    virtual IResample *createResample();
    virtual IVideoView *createVideoView();
    virtual IAudioPlay *createAudioPlay();
    virtual IPlayer *createPlayer(unsigned char index = 0);
};


#endif //XPLAY_FFPLAYERBUILDER_H
