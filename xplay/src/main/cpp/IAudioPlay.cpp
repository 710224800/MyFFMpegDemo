//
// Created by lyhao on 21-3-2.
//

#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::update(XData data) {
    //压入缓冲队列
    XLOGE("IAudioPlay::update data size is %d", data.size);
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