package com.example.myffdemo.features;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.example.myffdemo.LogUtil;
import com.example.myffdemo.NativeLib;
import com.example.myffdemo.R;

import java.io.File;

public class TestOpenActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_open_layout);
        String path = Environment.getExternalStorageDirectory().getPath() + "/1080.mp4";
        File file = new File(path);
        if(file.exists()){
            LogUtil.d("file exists");
        } else {
            LogUtil.d("file not exists");
        }
        NativeLib.getInstance().testFileOpen(path, this);

    }
}