//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_XEGL_H
#define XPLAY_XEGL_H


class XEGL {
public:
    virtual bool init(void *win) = 0;
    virtual void draw() = 0;
    static XEGL *get();

protected:
    XEGL(){}
};


#endif //XPLAY_XEGL_H
