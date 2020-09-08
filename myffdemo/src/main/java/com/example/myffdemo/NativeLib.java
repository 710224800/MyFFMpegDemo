package com.example.myffdemo;

/**
 * Created by luyanhao 20-9-8.
 */
public class NativeLib {
    static {
        System.loadLibrary("native-lib");
    }
    public native String getFFMpegConfiguration();
}
