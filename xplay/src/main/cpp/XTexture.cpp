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
    std::mutex cxTexture_mux;
    virtual void drop(){
        cxTexture_mux.lock();
        XEGL::get()->close();
        sh.close();
        cxTexture_mux.unlock();
        delete this;
    }
    virtual bool init(void *win, XType type){
        cxTexture_mux.lock();
        XEGL::get()->close();
        sh.close();
        this->type = type;
        XLOGE("XTexture init type is %d", type);
        if(win == nullptr){
            cxTexture_mux.unlock();
            XLOGE("XTexture init failed win is NULL");
            return false;
        }
        if(!XEGL::get()->init(win)){
            cxTexture_mux.unlock();
            return false;
        }
        sh.init(type);
        cxTexture_mux.unlock();
        return true;
    }
    virtual void draw(unsigned char *data[], int width, int height){
        cxTexture_mux.lock();
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
        cxTexture_mux.unlock();
    }
};

XTexture *XTexture ::create() {
    return new CXTexture();
}