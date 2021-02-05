//
// Created by lyhao on 21-2-5.
//

#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include "XEGL.h"
#include "XLog.h"

class CXEGL : public XEGL{
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    virtual bool init(void *win){
        ANativeWindow *nwin = (ANativeWindow *) win;

        //初始化EGL

        //1 获取EGLDisplay对象 显示设备
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(display == EGL_NO_DISPLAY){
            XLOGE("eglGetDisplay failed!");
            return false;
        }
        XLOGE("eglGetDisplay success!");
        //2 初始化Display
        if(EGL_TRUE != eglInitialize(display, nullptr, nullptr)){
            XLOGE("eglInitialize failed!");
            return false;
        }
        XLOGE("eglInitialize success!");

        //3 获取配置并创建surface
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };
        EGLConfig  config = nullptr;
        EGLint  numConfigs = 0;
        if(EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &numConfigs)){
            XLOGE("eglChooseConfig failed!");
            return false;
        }
        XLOGE("eglChooseConfig success!");
        surface = eglCreateWindowSurface(display, config, nwin, nullptr);

        //4 创建并打开EGL上下文
        const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if(context == EGL_NO_CONTEXT){
            XLOGE("eglCreateContext failed!");
            return false;
        }
        XLOGE("eglCreateContext success!");

        if(EGL_TRUE != eglMakeCurrent(display, surface, surface, context)){
            XLOGE("eglMakeCurrent failed!");
            return false;
        }
        XLOGE("eglMakeCurrent success!");

        return true;
    }
};

XEGL * XEGL::get() {
    static CXEGL egl;
    return &egl;
}