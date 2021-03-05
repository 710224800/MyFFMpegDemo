//
// Created by lyhao on 21-3-2.
//

#include "IResample.h"
#include "XLog.h"

void IResample::update(XData data) {
    XData d = this->resample(data);
    XLOGE("his->Resample(data) %d",d.size);
    if(d.size > 0){
        this->notify(d);
    }
}