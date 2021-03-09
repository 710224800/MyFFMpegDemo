//
// Created by lyhao on 20-10-10.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H
enum XDataType{
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1
};

struct XData {
    int type = AVPACKET_TYPE;
    unsigned char *data = nullptr;
    unsigned char *datas[8] = {0};
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;
    int format = 0;
    bool Alloc(int s, const char *d = nullptr);
    void Drop();
};


#endif //XPLAY_XDATA_H
