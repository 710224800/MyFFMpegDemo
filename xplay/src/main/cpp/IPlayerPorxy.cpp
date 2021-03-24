//
// Created by lyhao on 21-3-12.
//

#include "IPlayerPorxy.h"
#include "FFPlayerBuilder.h"

void IPlayerPorxy::init(void *vm) {
    mux.lock();
    if(vm){
        FFPlayerBuilder::initHard(vm);
    }
    mux.unlock();
}
bool IPlayerPorxy::open(const char *path) {
    bool re = false;
    mux.lock();
    if(!player){
        player = FFPlayerBuilder::get()->buildPlayer();
    }
    if(player){
        re = player->open(path);
    }
    mux.unlock();
    return re;
}
bool IPlayerPorxy::startPlay() {
    bool re = false;
    mux.lock();
    if(player){
        re = player->startPlay();
    }
    mux.unlock();
    return re;
}
void IPlayerPorxy::initView(void *win) {
    mux.lock();
    if(player){
        player->initView(win);
    }
    mux.unlock();
}
void IPlayerPorxy::release() {
    mux.lock();
    if(player){
        player->release();
        delete player;
        player = nullptr;
    }
}