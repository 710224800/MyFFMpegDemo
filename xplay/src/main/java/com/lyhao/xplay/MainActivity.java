package com.lyhao.xplay;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    public static String path1080mp4 = Environment.getExternalStorageDirectory().getPath() + "/1080.mp4";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView tv_ffmpeg_config = findViewById(R.id.tv_ffmpeg_config);
        tv_ffmpeg_config.setText(NativeLib.getInstance().getFFMpegConfig());

        NativeLib.getInstance().testIDemuxOpen(path1080mp4);
        tv_ffmpeg_config.postDelayed(new Runnable() {
            @Override
            public void run() {
                NativeLib.getInstance().testStop();
            }
        }, 3500);
    }
}
