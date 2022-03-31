package com.touchmcu.ui;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.view.View;

import androidx.annotation.NonNull;


public class DialogUtil {
    private static DialogUtil dialogUtil;
    private LoadingDialog loadingDialog;

    public static DialogUtil getInstance() {
        if(dialogUtil==null){
            synchronized (DialogUtil.class){
                dialogUtil=new DialogUtil();
            }
        }
        return dialogUtil;
    }



    public void showDisconnectDialog(@NonNull Activity activity,final IDisconnectResult result){
        AlertDialog.Builder builder=new AlertDialog.Builder(activity);

        builder .setMessage("蓝牙已连接，确定断开？")
                .setCancelable(false)
                .setPositiveButton("断开连接",null)
                .setNeutralButton("算了", null);

        final AlertDialog alertDialog = builder.create();
        alertDialog.show();
        alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onDisconnect();
                }
            }
        });
        alertDialog.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onCancel();
                }
            }
        });
    }

    public interface IDisconnectResult{
        void onDisconnect();
        void onCancel();
    }

    public interface IResult{
        void onContinue();
        void onCancel();
    }

    public void showLoadingDialog(@NonNull Activity activity,@NonNull String info){
        if(loadingDialog==null){
            loadingDialog=new LoadingDialog(activity,info);
            loadingDialog.show();
        }else {
            loadingDialog.setInfo(info);
        }

    }

    public void hideLoadingDialog(){
        if(loadingDialog!=null){
            loadingDialog.dismiss();
            loadingDialog=null;
        }
    }

    public void showSimpleDialog(@NonNull Activity activity,String message,final IResult result){
        AlertDialog.Builder builder=new AlertDialog.Builder(activity);

        builder .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("继续",null)
                .setNeutralButton("取消", null);

        final AlertDialog alertDialog = builder.create();
        alertDialog.show();
        alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onContinue();
                }
            }
        });
        alertDialog.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onCancel();
                }
            }
        });
    }

    public void showSimpleDialog(@NonNull Activity activity,String message,@NonNull String PositiveButtonText,@NonNull String NeutralButtonText,final IResult result){
        AlertDialog.Builder builder=new AlertDialog.Builder(activity);

        builder .setMessage(message)
                .setCancelable(false)
                .setPositiveButton(PositiveButtonText,null)
                .setNeutralButton(NeutralButtonText, null);

        final AlertDialog alertDialog = builder.create();
        alertDialog.show();
        alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onContinue();
                }
            }
        });
        alertDialog.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onCancel();
                }
            }
        });
    }

    public void showSimpleDialog(@NonNull Activity activity,String message,@NonNull String PositiveButtonText,final IResult result){
        AlertDialog.Builder builder=new AlertDialog.Builder(activity);

        builder .setMessage(message)
                .setCancelable(false)
                .setPositiveButton(PositiveButtonText,null);

        final AlertDialog alertDialog = builder.create();
        alertDialog.show();
        alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alertDialog.dismiss();
                if(result!=null){
                    result.onContinue();
                }
            }
        });
    }



}
