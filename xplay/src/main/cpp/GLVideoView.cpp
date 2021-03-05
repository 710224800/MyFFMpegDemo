//
// Created by lyhao on 21-2-5.
//

#include "GLVideoView.h"
#include "XTexture.h"
#include "XLog.h"

void GLVideoView::setRender(void *win) {
    view = win;
    if(txt != nullptr){
        XTexture *deleted = txt;
        txt = nullptr;
        delete deleted;
    }
}
void GLVideoView::render(XData data) {
    if(view == nullptr){
        return;
    }
    if(txt == nullptr){
        txt = XTexture::create();
        txt->init(view, (XType) data.format);
    }
    txt->draw(data.datas, data.width, data.height);
}