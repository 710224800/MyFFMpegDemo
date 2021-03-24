//
// Created by lyhao on 21-3-2.
//

#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::clear() {
    framesMutex.lock();
    while (!frames.empty()){
        frames.front().Drop();
        frames.pop_front();
    }
    framesMutex.unlock();
}

XData IAudioPlay::getData() {
    XData d;
    while (!isExit){
        framesMutex.lock();
        if(!frames.empty()){
            d = frames.front();
            frames.pop_front();
            framesMutex.unlock();
            pts = d.pts;
            return d;
        }
        framesMutex.unlock();
        XSleep(1);
    }
    return d;
}
void IAudioPlay::update(XData data) {
    //压入缓冲队列
    //XLOGE("IAudioPlay::update data size is %d", data.size);
    if(data.size <= 0 || data.data == nullptr){
        return;
    }
    while (!isExit){
        framesMutex.lock();
        if(frames.size() > maxFrame){
            XLOGE("IAudioPlay::frames.size() > %d", maxFrame);
            framesMutex.unlock();
            XSleep(1);
            continue;
        }
        frames.push_back(data);
        framesMutex.unlock();
        break;
    }
}