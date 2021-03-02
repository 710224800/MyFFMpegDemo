//
// Created by lyhao on 20-10-10.
//

#include "XData.h"
extern "C"{
#include <libavformat/avformat.h>
}
bool XData::Alloc(int s, const char *d) {
    Drop();
    type = UCHAR_TYPE;
    if(s <= 0){
        return false;
    }
    this->data = new unsigned char[s];
    if(this->data == nullptr){
        return false;
    }
    if(d != nullptr){
        memcpy(this->data, d, s);
    }
    return true;
}
void XData::Drop() {
    if (!data) return;
    if (type == AVPACKET_TYPE) {
        av_packet_free((AVPacket **) &data);
    } else {
        delete data;
    }
    data = nullptr;
    size = 0;
}