package com.example.myffdemo;

/**
 * Created by luyanhao 20-9-8.
 */
public class NativeLib {
    static {
        System.loadLibrary("native-lib");
    }
    private static volatile NativeLib INSTANCE = null;
    public static NativeLib getInstance(){
        if(INSTANCE == null){
            synchronized (NativeLib.class){
                if(INSTANCE == null){
                    INSTANCE = new NativeLib();
                }
            }
        }
        return INSTANCE;
    }

    public native String getFFMpegConfiguration();
    public native int testFileOpen(String url, Object handle);
    public native int avformatOpenInput(String url, Object surface);
    public native int openslTest(String url, Object handle);
    public native int openglTest(String url, Object handle);
}
