//
// Created by lyhao on 21-2-5.
//

#ifndef XPLAY_GLVIDEOVIEW_H
#define XPLAY_GLVIDEOVIEW_H


#include "XData.h"
#include "IVideoView.h"

class XTexture;

class GLVideoView : public IVideoView{
public:
    virtual void setRender(void *win);
    virtual void render(XData data);

protected:
    void *view = nullptr;
    XTexture *txt = nullptr;
};


#endif //XPLAY_GLVIDEOVIEW_H
