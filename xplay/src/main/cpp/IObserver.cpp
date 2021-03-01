//
// Created by lyhao on 21-1-18.
//

#include "IObserver.h"
#include "XLog.h"

void IObserver::addObs(IObserver *obs) {
    if(obs == nullptr){
        return;
    }
    mux.lock();
    obss.push_back(obs);
    XLOGI("addObs.size=%d", obss.size());
    mux.unlock();
}

void IObserver::notify(XData data) {
    mux.lock();
//    XLOGI("obss.size=%d", obss.size());
    for(auto & obs : obss){
        obs->update(data);
    }
    mux.unlock();
}