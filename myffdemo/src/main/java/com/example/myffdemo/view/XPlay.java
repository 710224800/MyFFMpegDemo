package com.example.myffdemo.view;

import android.content.Context;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.example.myffdemo.LogUtil;
import com.example.myffdemo.NativeLib;
import com.example.myffdemo.features.TestOpenActivity;

/**
 * Created by luyanhao on 20-9-21 14
 */
public class XPlay extends SurfaceView implements Runnable, SurfaceHolder.Callback{
    private SurfaceHolder mSurfaceHolder;
    public XPlay(Context context) {
        super(context);
    }

    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);

    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();
    }

    @Override
    public void run() {
        Surface surface = getHolder().getSurface();
        NativeLib.getInstance().avformatOpenInput(TestOpenActivity.path, surface);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        LogUtil.e("surfaceCreated");
        new Thread(XPlay.this).start();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }
}
