//
// Created by lyhao on 20-10-12.
//

#include "XThread.h"
#include "XLog.h"

#include <thread>
using namespace std;

void XThread::Start() {
    thread th(&XThread::ThreadMain, this);
    th.detach();
}

void XThread::ThreadMain() {
    XLOGI("线程函数进入");
    Main();
    XLOGI("线程函数退出");
}

//通过判断isExit安全停止线程（不一定成功）
void XThread::Stop() {

}
