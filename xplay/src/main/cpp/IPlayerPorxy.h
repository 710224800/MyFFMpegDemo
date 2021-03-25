//
// Created by lyhao on 21-3-12.
//

#ifndef XPLAY_IPLAYERPORXY_H
#define XPLAY_IPLAYERPORXY_H


#include "IPlayer.h"
#include <mutex>

class IPlayerPorxy : public IPlayer{
public:
    static IPlayerPorxy *get(){
        static IPlayerPorxy porxy;
        return &porxy;
    }
    void init(void *vm = 0);
    virtual bool open(const char *path);
    virtual bool seek(double pos);
    virtual void close();
    virtual bool startPlay();
    virtual void initView(void *win);
    virtual void setPause(bool isP);
    virtual bool isPause();
    //当前的播放进度，0.0 ~ 1.0
    virtual double playPos();

    virtual void release();

protected:
    IPlayerPorxy(){}
    IPlayer *player = nullptr;
    std::mutex mux;

};


#endif //XPLAY_IPLAYERPORXY_H
