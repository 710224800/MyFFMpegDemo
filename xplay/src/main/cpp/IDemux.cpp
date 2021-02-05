//
// Created by lyhao on 20-10-10.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::main() {
    XData d = read();
    while(!isExit){
        XLOGI("IDemux main Read data size is %d", d.size);
        d = read();
        if(d.size != 0){
            notify(d);
        }
        if(d.size == -9999){
            notify(d);
            stop();
        }
    }
}