//
// Created by lyhao on 21-2-5.
//

#include "XTexture.h"
#include "XShader.h"
#include "XLog.h"
#include "XEGL.h"

class CXTexture : public XTexture{
public:
    XShader sh;
    virtual bool init(void *win){
        if(win == nullptr){
            XLOGE("XTexture init failed win is NULL");
            return false;
        }
        if(!XEGL::get()->init(win)){
            return false;
        }
        sh.init();
        return true;
    }
};

XTexture *XTexture ::create() {
    return new CXTexture();
}