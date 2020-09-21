package com.example.myffdemo.features;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.widget.LinearLayout;

import com.example.myffdemo.R;
import com.example.myffdemo.util.ScreenUtil;

public class XplayActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_xplay);
        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface_view);
        LinearLayout.LayoutParams params = (LinearLayout.LayoutParams) surfaceView.getLayoutParams();
        params.width = LinearLayout.LayoutParams.MATCH_PARENT;
        params.height = (int) (720f / 1280f * ScreenUtil.getScreenWidth(this));
    }
}