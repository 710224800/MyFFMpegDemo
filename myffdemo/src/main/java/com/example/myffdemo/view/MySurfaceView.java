package com.example.myffdemo.view;

import android.content.Context;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.example.myffdemo.LogUtil;
import com.example.myffdemo.NativeLib;

/**
 * Created by luyanhao on 20-9-28 19
 */
public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    public MySurfaceView(Context context) {
        this(context, null);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        getHolder().addCallback(this);
    }
    public static String yuvFile = Environment.getExternalStorageDirectory().getPath() + "/out_480x360.yuv";
    @Override
    public void surfaceCreated(@NonNull final SurfaceHolder holder) {
        LogUtil.e("MySurfaceView surfaceCreated");
        new Thread(new Runnable() {
            @Override
            public void run() {
                NativeLib.getInstance().openglTest(yuvFile, holder.getSurface());
            }
        }).start();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }
}
