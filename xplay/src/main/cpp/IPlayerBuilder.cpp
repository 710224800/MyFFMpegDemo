//
// Created by lyhao on 21-3-11.
//

#include "IPlayerBuilder.h"

IPlayer * IPlayerBuilder::buildPlayer(unsigned char index) {
    IPlayer *play = createPlayer(index);

    IDemux *de = createDemux();
    IDecode *vdecode = createDecode();
    IDecode *adecode = createDecode();

    de->addObs(vdecode);
    de->addObs(adecode);

    IVideoView *view = createVideoView();
    vdecode->addObs(view);

    IResample *resample = createResample();
    adecode->addObs(resample);

    IAudioPlay *audioPlay = createAudioPlay();
    resample->addObs(audioPlay);

    play->demux = de;
    play->vdecode = vdecode;
    play->adecode = adecode;
    play->videoView = view;
    play->resample = resample;
    play->audioPlay = audioPlay;
    return play;
}