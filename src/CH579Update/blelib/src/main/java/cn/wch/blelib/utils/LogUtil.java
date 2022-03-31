package cn.wch.blelib.utils;

import android.util.Log;

public class LogUtil {
    public static String TAG="BLELOG";
    public static void d(String msg){
        Log.d(TAG,msg);
    }

    public static void d(String type,String mac,String msg){
        Log.d(TAG,type+" "+mac+" "+msg);
    }
}
