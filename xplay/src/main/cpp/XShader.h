//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H


class XShader {
public:
    virtual bool init();

    //获取材质并映射到内存
    virtual void getTexture(unsigned int index, int width, int height, unsigned char *buf);
    virtual void draw();
protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int texts[100] = {0};
};


#endif //XPLAY_XSHADER_H
