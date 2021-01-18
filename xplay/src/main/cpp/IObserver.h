//
// Created by lyhao on 21-1-18.
//
//
// Created by lyhao on 21-1-18.
//

#ifndef XPLAY_IOBSERVER_H
#define XPLAY_IOBSERVER_H

#include "XThread.h"
#include "XData.h"
#include <vector>
#include <mutex>

class IObserver : public XThread{

public:
    // 观察者接收数据 函数
    virtual void update(XData data){}

    // 添加观察者（线程安全）
    void addObs(IObserver *obs);

    // 通知所有观察者（线程安全）
    void notify(XData data);

protected:
    std::vector<IObserver *>obss;
    std::mutex mux;

};
#endif //XPLAY_IOBSERVER_H