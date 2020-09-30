#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "native-lib", __VA_ARGS__)
#define GET_STR(x) #x

//顶点着色器glsl
static const char *vertexShader = GET_STR(
    attribute vec4 aPosition; // 顶点坐标
    attribute vec2 aTexCoord; // 材质顶点坐标
    varying vec2 vTexCoord;
    void main(){
        vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
        gl_Position = aPosition;
    }
);

static const char *fragYUV420P = GET_STR(
    precision mediump float; // 精度
    varying vec2 vTexCoord; // 顶点着色器传递的坐标
    uniform sampler2D yTexture; // 输入的材质（不透明灰度，单像素）
    uniform sampler2D uTexture;
    uniform sampler2D vTexture;
    void main(){
        vec3 yuv;
        vec3 rgb;
        yuv.r = texture2D(yTexture, vTexCoord).r;
        yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
        yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
        rgb = mat3(1.0,     1.0,    1.0,
                   0.0, -0.39465, 2.03211,
                   1.13983, -0.58060, 0.0) * yuv;
        gl_FragColor = vec4(rgb, 1.0);
    }
);

GLint InitShader(const char *code, GLint type){
    //创建shader
    GLint sh = glCreateShader(type);
    if(sh == 0){
        LOGE("InitShader failed!");
        return 0;
    }
    //加载shader
    glShaderSource(sh, 1, &code, 0);
    //编译shader
    glCompileShader(sh);
    //获取编译情况
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if(status == 0){
        LOGE("glCompileShader failed");
        return 0;
    }
    LOGE("glCompileShader success");
    return sh;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_myffdemo_NativeLib_openglTest(JNIEnv *env, jobject thiz, jstring url,
                                               jobject surface) {
    LOGE("openGLTest");
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);

    //1. EGL display创建和初始化
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display == EGL_NO_DISPLAY){
        LOGE("eglGetDisplay failed");
        return -1;
    }
    if(EGL_TRUE != eglInitialize(display, 0, 0)){
        LOGE("eglInitialize failed");
        return -1;
    }
    //2.1 surface窗口配置
    EGLConfig config;
    EGLint configNum;
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
    };

    if(EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &configNum)){
        LOGE("eglChooseConfig failed");
        return -1;
    }

    //2.2创建surface
    EGLSurface winsurface = eglCreateWindowSurface(display, config, nwin, 0);
    if(winsurface == EGL_NO_SURFACE){
        LOGE("eglCreateWindowSurface failed");
        return -1;
    }
    //2.3 context 创建关联上下文
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
    if(context == EGL_NO_CONTEXT){
        LOGE("eglCreateContext failed");
        return -1;
    }
    if(EGL_TRUE != eglMakeCurrent(display, winsurface, winsurface, context)){
        LOGE("eglMakeCurrent failed");
        return -1;
    }
    LOGE("EGL int success");

    //顶点和片元shader初始化
    //顶点shader
    GLint vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    //片元shader
    GLint fsh = InitShader(fragYUV420P, GL_FRAGMENT_SHADER);


    return 9090;
}
