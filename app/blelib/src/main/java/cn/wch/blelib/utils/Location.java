package cn.wch.blelib.utils;

import android.content.Context;
import android.content.Intent;
import android.location.LocationManager;
import android.os.Build;
import android.provider.Settings;

import androidx.annotation.RequiresApi;

/**
 * 安卓位置服务，在Android 10及以上，如果不开启，则无法扫描到设备
 */
public class Location {
    @RequiresApi(api = Build.VERSION_CODES.P)
    public static boolean isLocationEnable(Context context){

        LocationManager locationManager=(LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        if(locationManager==null){
            return false;
        }
        return locationManager.isLocationEnabled();
    }

    public static void requestLocationService(Context context){
        context.startActivity(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS));
    }
}
