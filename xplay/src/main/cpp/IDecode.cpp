//
// Created by lyhao on 21-1-20.
//

#include "IDecode.h"
#include "XLog.h"

void IDecode::update(XData pkt) {
    if(pkt.size == -9999){
        stop(); // 到文件结尾了，结束
        return;
    }
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
        //XLOGI("packs.size = %d isAudio=%d", packs.size(), isAudio);
        packsMutex.unlock();
        XSleep(1);
    }
}

void IDecode::clear() {
    packsMutex.lock();
    while (!packs.empty()){
        packs.front().Drop();
        packs.pop_front();
    }
    pts = 0;
    synPts = 0;
    packsMutex.unlock();
}

void IDecode::main() {
    while (!isExit){
//        XLOGI("running isAudio=%d", isAudio);
        packsMutex.lock();

        //判断音视频同步
        if(!isAudio && synPts > 0){
            if(synPts < pts){
                packsMutex.unlock();
                XSleep(1);
                continue;
            }
        }

        if(packs.empty()){
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        //消费者
        XData pkt = packs.front();
        packs.pop_front();
        //XLOGI("sendPacket isAudio=%d pts=%d", pkt.isAudio, pkt.pts);
        if(this->sendPacket(pkt)){
            while (!isExit){
                XData frame = recvFrame();
//                XLOGI("recvFrame frame %d frame size=%d", frame.isAudio, frame.size);
                if(frame.data == nullptr){
                    break;
                }
                pts = frame.pts;
                //XLOGI("recvFrame isAudio=%d pts=%d", pkt.isAudio, frame.pts);
//                XLOGI("notify frame %d", frame.isAudio);
                this->notify(frame);
            }
        }
        pkt.Drop();//...
        packsMutex.unlock();
    }
}