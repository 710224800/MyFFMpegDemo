//
// Created by lyhao on 20-10-10.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::main() {
    XData d = Read();
    while(!isExit){
        XLOGI("IDemux main Read data size is %d", d.size);
        d = Read();
        if(d.size != 0){
            notify(d);
        }
        if(d.size == -9999){
            notify(d);
            Stop();
        }
    }
}