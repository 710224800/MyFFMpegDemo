package com.example.myffdemo;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import com.example.myffdemo.features.GLSurfaceActivity;
import com.example.myffdemo.features.TestOpenActivity;
import com.example.myffdemo.features.XplayActivity;

public class FetureListActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_feture_list);
    }

    public void onTestOpen(View v){
        startActivity(new Intent(this, TestOpenActivity.class));
    }

    public void onTestXplay(View view) {
        startActivity(new Intent(this, XplayActivity.class));
    }

    public void onTestGlsurfaceView(View view) {
        startActivity(new Intent(this, GLSurfaceActivity.class));
    }
}