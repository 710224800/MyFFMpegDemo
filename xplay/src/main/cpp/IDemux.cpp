//
// Created by lyhao on 20-10-10.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    XData d = Read();
    while(!isExit){
        XLOGI("Read data size is %d", d.size);
        d = Read();
    }
}