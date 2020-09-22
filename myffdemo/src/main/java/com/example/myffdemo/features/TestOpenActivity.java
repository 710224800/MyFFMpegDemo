package com.example.myffdemo.features;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import com.example.myffdemo.LogUtil;
import com.example.myffdemo.NativeLib;
import com.example.myffdemo.R;

import java.io.File;

public class TestOpenActivity extends AppCompatActivity {

    public static String path = Environment.getExternalStorageDirectory().getPath() + "/1080.mp4";
    public static String pcmFile = Environment.getExternalStorageDirectory().getPath() + "/test.pcm";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_open_layout);
        File file = new File(path);
        if(file.exists()){
            LogUtil.d("file exists");
        } else {
            LogUtil.d("file not exists");
        }
        NativeLib.getInstance().testFileOpen(path, this);
    }

    public void onOpenSlTest(View v){
        int i = NativeLib.getInstance().openslTest(pcmFile, this);
    }
}