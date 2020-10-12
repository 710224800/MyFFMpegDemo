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

void XThread::Start() {
    isExit = false;
    thread th(&XThread::ThreadMain, this);
    th.detach();
}

void XThread::ThreadMain() {
    isRunning = true;
    XLOGI("线程函数进入");
    Main();
    XLOGI("线程函数退出");
    isRunning = false;
}

//通过判断isExit安全停止线程（不一定成功）
void XThread::Stop() {
    isExit = true;
    for(int i = 0; i < 200; i++)
    {
        if(!isRunning)
        {
            XLOGI("Stop 停止线程成功!");
            return;
        }
        XSleep(1);
    }
    XLOGI("Stop 停止线程超时!");
}
