//
// Created by lyhao on 21-3-9.
//

#include "IPlayer.h"
#include "XLog.h"

IPlayer * IPlayer::get(unsigned char index) {
    static IPlayer iPlayer[256];
    return &iPlayer[index];
}

void IPlayer::main() {
    while (!isExit){
        mux.lock();
        if(!audioPlay || !vdecode){
            mux.unlock();
            XSleep(2);
            continue;
        }
        //同步
        //获取音频的pts,告诉视频
        int apts = audioPlay->pts;
        XLOGE("apts = %d", apts);
        vdecode->synPts = apts;

        mux.unlock();
        XSleep(2);
    }
}

void IPlayer::close()
{
    mux.lock();
    //2 先关闭主体线程，再清理观察者
    //同步线程
    XThread::stop();
    //解封装
    if(demux)
        demux->stop();
    //解码
    if(vdecode)
        vdecode->stop();
    if(adecode)
        adecode->stop();
    //2 清理缓冲队列
    if(vdecode)
        vdecode->clear();
    if(adecode)
        adecode->clear();
    if(audioPlay)
        audioPlay->clear();

    //3 清理资源
    if(audioPlay)
        audioPlay->close();
    if(videoView)
        videoView->close();
    if(vdecode)
        vdecode->close();
    if(adecode)
        adecode->close();
    if(demux)
        demux->close();
    mux.unlock();
}

bool IPlayer::open(const char *path) {
    close();
    mux.lock();
    //解封装
    if (demux == nullptr || !demux->open(path)) {
        mux.unlock();
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
    mux.unlock();
    return true;
}
bool IPlayer::startPlay()
{
    mux.lock();
    if(demux == nullptr || !demux->start())
    {
        mux.unlock();
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
    XThread::start();
    mux.unlock();
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