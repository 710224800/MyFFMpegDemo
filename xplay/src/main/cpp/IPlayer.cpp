//
// Created by lyhao on 21-3-9.
//

#include "IPlayer.h"
#include "XLog.h"

IPlayer * IPlayer::get(unsigned char index) {
    static IPlayer iPlayer[256];
    return &iPlayer[index];
}

bool IPlayer::open(const char *path) {
    //解封装
    if (demux == nullptr || !demux->open(path)) {
        XLOGE("demux->Open %s failed!", path);
        return false;
    }
    XParameter vPara = demux->getVPara();
    XParameter aPara = demux->getAPara();
    //解码 解码可能不需要，如果是解封之后就是原始数据
    if (vdecode == nullptr || !vdecode->open(vPara, isHardDecode)) {
        XLOGE("vdecode->Open %s failed!", path);
        //return false;
    }
    if (adecode == nullptr || !adecode->open(aPara)) {
        XLOGE("adecode->Open %s failed!", path);
        //return false;
    }

    //重采样 有可能不需要，解码后或者解封后可能是直接能播放的数据
//    if (outPara.sample_rate <= 0) {
        outPara = demux->getAPara();
//    }

    if(resample == nullptr || !resample->open(aPara, outPara))
    {
        XLOGE("resample->Open %s failed!", path);
    }
    return true;
}
bool IPlayer::startPlay()
{
    if(demux == nullptr || !demux->start())
    {
        XLOGE("demux->Start failed!");
        return false;
    }
    if(adecode != nullptr) {
        adecode->start();
    }
    if(audioPlay != nullptr) {
        audioPlay->startPlay(outPara);
    }
    if(vdecode != nullptr){
        vdecode->start();
    }
    return true;
}

void IPlayer::initView(void *win) {
    if(videoView != nullptr){
        videoView->setRender(win);
    }
}

void IPlayer::release() {
    if(audioPlay){
        delete audioPlay;
        audioPlay = nullptr;
    }
    if(demux){
        demux->stop();
        delete demux;
        demux = nullptr;
    }
    if(adecode){
        adecode->stop();
        delete adecode;
        adecode = nullptr;
    }
    if(vdecode){
        vdecode->stop();
        delete vdecode;
        vdecode = nullptr;
    }
    if(videoView){
        delete videoView;
        videoView = nullptr;
    }
    if(resample){
        delete resample;
        resample = nullptr;
    }
}