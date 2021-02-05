package com.lyhao.xplay;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

public class XPlay extends GLSurfaceView implements SurfaceHolder.Callback {
    public XPlay(Context context) {
        this(context, null);
    }

    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        //初始化opengl egl 显示
        NativeLib.getInstance().initView(holder);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        super.surfaceChanged(holder, format, w, h);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
    }
}
