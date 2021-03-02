//
// Created by lyhao on 21-3-2.
//

#include "IResample.h"

void IResample::update(XData data) {
    XData d = this->resample(data);
    if(d.size > 0){
        this->notify(data);
    }
}