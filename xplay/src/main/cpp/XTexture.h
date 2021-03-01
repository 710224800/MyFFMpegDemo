//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_XTEXTURE_H
#define XPLAY_XTEXTURE_H


class XTexture {
public:
    static XTexture *create();
    virtual bool init(void *win) = 0;
};


#endif //XPLAY_XTEXTURE_H
