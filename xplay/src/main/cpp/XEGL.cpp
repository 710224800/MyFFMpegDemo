//
// Created by lyhao on 21-2-5.
//

#include "XEGL.h"
class CXEGL : public XEGL{
public:
    virtual bool init(void *win){
        return true;
    }
};

XEGL * XEGL::get() {
    static CXEGL egl;
    return &egl;
}