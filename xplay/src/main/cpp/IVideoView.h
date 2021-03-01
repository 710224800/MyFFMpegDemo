//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_IVIDEOVIEW_H
#define XPLAY_IVIDEOVIEW_H


#include "XData.h"

class IVideoView {
public:
    virtual void setRender(void *win) = 0;
    virtual void render(XData data) = 0;
    virtual void update(XData data);
};


#endif //XPLAY_IVIDEOVIEW_H
