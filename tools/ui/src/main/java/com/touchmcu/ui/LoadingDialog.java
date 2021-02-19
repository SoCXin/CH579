package com.touchmcu.ui;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.TextView;

import androidx.annotation.NonNull;

public class LoadingDialog extends Dialog {


    private String textIndicator="";
    private Handler handler=new Handler(Looper.getMainLooper());

    public LoadingDialog(@NonNull Context context) {
        super(context, R.style.CustomDialog);
    }

    public LoadingDialog(@NonNull Context context,String info) {
        super(context, R.style.CustomDialog);
        textIndicator=info;
    }


    TextView info;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dialog_loading);
        setCancelable(false);
        init();
    }

    private void init() {
        info=findViewById(R.id.tv_loading_info);
        info.setText(textIndicator);
    }

    public void setInfo(final String text){
        handler.post(new Runnable() {
            @Override
            public void run() {
                textIndicator=text;
                info.setText(text);
            }
        });
    }

}
