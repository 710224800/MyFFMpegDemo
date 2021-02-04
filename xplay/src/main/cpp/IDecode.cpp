//
// Created by lyhao on 21-1-20.
//

#include "IDecode.h"
#include "XLog.h"

void IDecode::update(XData pkt) {
    if(pkt.isAudio != isAudio){ // 视频解码器只解视频，音频解码器只解音频
        return;
    }
    while (!isExit){ // 线程未退出
        packsMutex.lock();
        if(packs.size() < maxList){
            //生产者
            packs.push_back(pkt);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        XSleep(1);
    }
}

void IDecode::main() {
    while (!isExit){
        packsMutex.lock();
        if(packs.empty()){
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        //消费者
        XData pkt = packs.front();
        packs.pop_front();

        if(this->sendPacket(pkt)){
            while (!isExit){
                XData frame = recvFrame();
                if(frame.data == nullptr){
                    break;
                }
                XLOGI("notify frame %d", frame.isAudio);
                this->notify(frame);
            }
        }
        pkt.Drop();//...
        packsMutex.unlock();
    }
}