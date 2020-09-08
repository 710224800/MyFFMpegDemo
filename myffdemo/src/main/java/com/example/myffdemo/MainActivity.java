package com.example.myffdemo;

import android.content.Intent;
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

        NativeLib native_lib = new NativeLib();

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
    }

}
