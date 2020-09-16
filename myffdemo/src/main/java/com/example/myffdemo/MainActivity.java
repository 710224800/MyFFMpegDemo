package com.example.myffdemo;

import android.Manifest;
import android.content.Intent;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        NativeLib native_lib = NativeLib.getInstance();

        TextView text_view = (TextView) findViewById(R.id.text_view);
        text_view.setText(native_lib.getFFMpegConfiguration());

        Button gotoFeatureList = (Button) findViewById(R.id.gotoList);
        gotoFeatureList.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, FetureListActivity.class);
                startActivity(intent);
            }
        });
        requestPermission();
    }

    private String[] permissions = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE};

    /**
     * 请求授权
     */
    private void requestPermission(){
        ActivityCompat.requestPermissions(this, permissions,1);
    }

}
