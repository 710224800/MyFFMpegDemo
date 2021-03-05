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
    XType type;
    virtual bool init(void *win, XType type){
        this->type = type;
        XLOGE("XTexture init type is %d", type);
        if(win == nullptr){
            XLOGE("XTexture init failed win is NULL");
            return false;
        }
        if(!XEGL::get()->init(win)){
            return false;
        }
        sh.init(type);
        return true;
    }
    virtual void draw(unsigned char *data[], int width, int height){
        sh.getTexture(0, width, height, data[0]); //Y
        switch (type) {
            case XTEXTURE_YUV420P:
                sh.getTexture(1, width/2, height/2, data[1]); //U
                sh.getTexture(2, width/2, height/2, data[2]); //V
                break;
            case XTEXTURE_NV12:
            case XTEXTURE_NV21:
                sh.getTexture(1, width/2, height/2, data[1], true); //UV
                break;
        }
        sh.draw();
        XEGL::get()->draw();
    }
};

XTexture *XTexture ::create() {
    return new CXTexture();
}