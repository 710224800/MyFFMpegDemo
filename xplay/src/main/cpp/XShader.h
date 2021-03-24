//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H


#include <mutex>
#include "XTextureType.h"

class XShader {
public:
    virtual bool init(XType type = XTEXTURE_YUV420P);
    virtual void close();

    //获取材质并映射到内存
    virtual void getTexture(unsigned int index, int width, int height, unsigned char *buf, bool isa=false);
    virtual void draw();
protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int texts[100] = {0};
    std::mutex xShader_mux;
};


#endif //XPLAY_XSHADER_H
