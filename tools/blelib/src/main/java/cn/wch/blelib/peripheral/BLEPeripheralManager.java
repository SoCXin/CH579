package cn.wch.blelib.peripheral;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.content.Context;
import android.os.Build;

import androidx.annotation.RequiresApi;
import cn.wch.blelib.exception.BLELibException;


public class BLEPeripheralManager {
    private static BLEPeripheralManager mThis;
    private Linker linker;
    private BluetoothGattServer server;

    public static BLEPeripheralManager getInstance(){
        if(mThis==null){
            synchronized (BLEPeripheralManager.class){
                mThis=new BLEPeripheralManager();
            }
        }
        return mThis;
    }

    public void init(Context application,BluetoothGattServerCallback callback) throws BLELibException {
        if(BluetoothAdapter.getDefaultAdapter()==null) {
            throw new BLELibException("BluetoothAdapter is null");
        }
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP){
            throw new BLELibException("该功能仅支持 Android 5.0及以上系统");
        }else {
            if (!BluetoothAdapter.getDefaultAdapter().isMultipleAdvertisementSupported()) {
                throw new BLELibException("该设备不支持广播功能");
            }
        }
        linker=new Linker(application,callback);
        server=linker.getmGattServer();
    }

    /**
     * 添加自定义服务
     * @param service
     */
    public void addService(BluetoothGattService service){
        if(linker==null || service==null){
            return;
        }
        linker.addService(service);
    }

    /**
     * 清理服务
     */
    public void clearServices(){
        if(linker==null ){
            return;
        }
        linker.clearServices();
    }

    /**
     * 开始广播
     * @param settings
     * @param data
     * @param scanResponse
     * @param callback
     */
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void startAdvertise(AdvertiseSettings settings, AdvertiseData data, AdvertiseData scanResponse, AdvertiseCallback callback){
        if(linker==null ){
            return;
        }
        linker.startAdvertise(settings, data, scanResponse, callback);
    }

    /**
     * 停止广播
     */
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void stopAdvertise(){
        if(linker==null ){
            return;
        }
        linker.stopAdvertise();
    }


    public void close(){
        if(linker==null ){
            return;
        }
        linker.close();
    }
    public BluetoothGattServer getServer() {
        return server;
    }
}
