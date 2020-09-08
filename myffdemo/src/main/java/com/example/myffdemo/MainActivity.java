package com.example.myffdemo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        NativeLib native_lib = new NativeLib();

        TextView text_view = (TextView) findViewById(R.id.text_view);
        text_view.setText(native_lib.stringFromJNI());
    }

}
