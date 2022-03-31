package cn.wch.blelib.host.core;
/**
 * 2019.07.24 v1.2.0 cn.wch.zpc
 */

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.os.Build;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.host.core.callback.ConnectCallback;
import cn.wch.blelib.host.scan.ScanRuler;
import cn.wch.blelib.utils.BLEUtil;

public class BLEHostManager {
    private static BLEHostManager mThis;
    public  Context context;
    public static BluetoothManager bluetoothManager;
    private ConnPresent connPresent;

    private BLEHostManager(Context context) {
        connPresent=ConnPresent.getInstance(context);
    }

    public static BLEHostManager getInstance(Context context){
        if(mThis==null){
            synchronized (BLEHostManager.class){
                mThis=new BLEHostManager(context);
            }
        }
        return mThis;
    }

    public synchronized void init(Application application)throws BLELibException {
        if(application!=null){
            context=application;
            if(BLEUtil.isSupportBLE(context)) {
                bluetoothManager = (BluetoothManager) (context.getSystemService(Context.BLUETOOTH_SERVICE));
            }else{
                throw new BLELibException("this device doesn't support BLE");
            }
        }else{
            throw new BLELibException("Application is null");
        }
    }



    public synchronized void asyncConnect(@NonNull ConnRuler connRuler, @NonNull ConnectCallback callback){
        connPresent.asyncConnect(connRuler,callback);
    }

    public synchronized void disconnect(@NonNull String mac)throws BLELibException {
        connPresent.disconnect(mac);
    }

    public synchronized void close(@NonNull String mac)throws BLELibException {
        connPresent.close(mac);
    }

    public synchronized void closeAll(){
        connPresent.closeAll();
    }

    public synchronized boolean isConnected(@NonNull String mac){
        return connPresent.isConnected(mac);
    }

    public synchronized Connection getConnection(String mac){
        return connPresent.getConnection(mac);
    }

    public HashMap<String, Connection> getConnDevice(){
        return connPresent.getConnDevice();
    }




}
