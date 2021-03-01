//
// Created by lyhao on 20-10-10.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H


struct XData {
    unsigned char *data = nullptr;
    unsigned char *datas[8] = {0};
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;
    void Drop();
};


#endif //XPLAY_XDATA_H
