//
// Created by lyhao on 21-2-5.
//

#include "GLVideoView.h"
#include "XTexture.h"
void GLVideoView::setRender(void *win) {
    view = win;
}
void GLVideoView::render(XData data) {
    if(view == nullptr){
        return;
    }
    if(txt == nullptr){
        txt = XTexture::create();
        txt->init(view);
    }
}