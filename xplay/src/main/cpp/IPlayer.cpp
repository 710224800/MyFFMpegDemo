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
    if(demux == nullptr || !demux->open(path)){
        XLOGE("demux->Open %s failed!", path);
        return false;
    }
    XParameter vPara = demux->getVPara();
    XParameter aPara = demux->getAPara();
    //解码 解码可能不需要，如果是解封之后就是原始数据
    if(vdecode == nullptr || !vdecode->open(vPara, isHardDecode))
    {
        XLOGE("vdecode->Open %s failed!", path);
        //return false;
    }
    if(adecode == nullptr || !adecode->open(aPara))
    {
        XLOGE("adecode->Open %s failed!", path);
        //return false;
    }

    //重采样 有可能不需要，解码后或者解封后可能是直接能播放的数据
    XParameter outPara = demux->getAPara();

    if(resample == nullptr || !resample->open(aPara, outPara))
    {
        XLOGE("resample->Open %s failed!", path);
    }
    return true;
}
bool IPlayer::startPlay()
{
    return true;
}