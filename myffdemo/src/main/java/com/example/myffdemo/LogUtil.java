package com.example.myffdemo;

import android.util.Log;

/**
 * Created by luyanhao on 20-9-16 11
 */
public class LogUtil {
    private static final String TAG = "myffmepg";
    public static void d(String s){
        Log.d(TAG, s);
    }

    public static void d(String TAG, String s){
        Log.d(TAG, s);
    }

    public static void e(String s){
        Log.e(TAG, s);
    }

    public static void e(String TAG, String s){
        Log.e(TAG, s);
    }
}
