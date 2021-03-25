//
// Created by lyhao on 20-10-12.
//

#include "XThread.h"
#include "XLog.h"

#include <thread>
using namespace std;

void XSleep(int mis){
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

void XThread::setPause(bool isP) {
    isPause = isP;
    //等待100毫秒
    for(int i = 0; i < 10; i++)
    {
        if(isPausing == isP)
        {
            break;
        }
        XSleep(10);
    }
}

bool XThread::start() {
    isExit = false;
    thread th(&XThread::threadMain, this);
    th.detach();
    return true;
}

void XThread::threadMain() {
    isRunning = true;
    XLOGI("线程函数进入");
    main();
    XLOGI("线程函数退出");
    isRunning = false;
}

//通过判断isExit安全停止线程（不一定成功）
void XThread::stop() {
    if(isExit){ // 已经 停止
        return;
    }
    isExit = true;
    for(int i = 0; i < 200; i++)
    {
        if(!isRunning)
        {
            XLOGI("stop 停止线程成功!");
            return;
        }
        XSleep(1);
    }
    XLOGI("stop 停止线程超时!");
}
