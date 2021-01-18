//
// Created by lyhao on 21-1-18.
//

#include "IObserver.h"

void IObserver::addObs(IObserver *obs) {
    if(obs == nullptr){
        return;
    }
    mux.lock();
    obss.push_back(obs);
    mux.unlock();
}

void IObserver::notify(XData data) {
    mux.lock();
    for(auto & obs : obss){
        obs->update(data);
    }
    mux.unlock();
}