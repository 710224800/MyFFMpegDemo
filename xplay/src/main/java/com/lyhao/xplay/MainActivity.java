package com.lyhao.xplay;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView tv_ffmpeg_config = findViewById(R.id.tv_ffmpeg_config);
        tv_ffmpeg_config.setText(NativeLib.getInstance().getFFMpegConfig());
    }
}
