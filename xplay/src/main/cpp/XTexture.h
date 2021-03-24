//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_XTEXTURE_H
#define XPLAY_XTEXTURE_H


#include "XTextureType.h"

class XTexture {
public:
    static XTexture *create();
    virtual bool init(void *win, XType type = XTEXTURE_YUV420P) = 0;
    virtual void draw(unsigned char *data[], int width, int height) = 0;
    virtual void drop() = 0;
    virtual ~XTexture(){};
protected:
    XTexture(){};
};


#endif //XPLAY_XTEXTURE_H
