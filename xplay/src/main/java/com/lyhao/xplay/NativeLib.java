package com.lyhao.xplay;

/**
 * Created by luyanhao on 20-10-10 10
 */
public class NativeLib {
    static {
        System.loadLibrary("native-lib");
    }
    private static volatile NativeLib INSTANCE;
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

    public native String getFFMpegConfig();
}
