//
// Created by lyhao on 21-3-11.
//

#include "FFPlayerBuilder.h"
#include "FFDemux.h"
#include "GLVideoView.h"
#include "FFDecode.h"
#include "FFResample.h"
#include "SLAudioPlay.h"

IDemux * FFPlayerBuilder::createDemux() {
    IDemux *de = new FFDemux();
    return de;
}
IDecode * FFPlayerBuilder::createDecode() {
    IDecode *decode = new FFDecode();
    return decode;
}
IResample * FFPlayerBuilder::createResample() {
    IResample *resample = new FFResample();
    return resample;
}
IVideoView * FFPlayerBuilder::createVideoView() {
    IVideoView *view = new GLVideoView();
    return view;
}
IAudioPlay * FFPlayerBuilder::createAudioPlay() {
    IAudioPlay *audioPlay = new SLAudioPlay();
    return audioPlay;
}
IPlayer * FFPlayerBuilder::createPlayer(unsigned char index) {
    return IPlayer::get(index);
}
void FFPlayerBuilder::initHard(void *vm) {
    FFDecode::initHard(vm);
}