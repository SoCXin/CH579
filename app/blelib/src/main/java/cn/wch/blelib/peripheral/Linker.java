package cn.wch.blelib.peripheral;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.content.Context;
import android.os.Build;

import androidx.annotation.RequiresApi;
import cn.wch.blelib.utils.LogUtil;

public class Linker {
    private Context mContext=null;
    private BluetoothLeAdvertiser mBluetoothAdvertiser;
    private AdvertiseCallback advertiseCallback;
    private BluetoothGattServer mGattServer;
    private BluetoothGattServerCallback mMockServerCallBack;

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter=BluetoothAdapter.getDefaultAdapter();

    public Linker (Context context,BluetoothGattServerCallback callBack){
        mContext=context;
        mMockServerCallBack=callBack;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            init(callBack);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void init(BluetoothGattServerCallback callBack){
        if (mBluetoothAdvertiser == null) {
            mBluetoothAdvertiser = BluetoothAdapter.getDefaultAdapter().getBluetoothLeAdvertiser();
        }
        if(mBluetoothManager==null) {
            mBluetoothManager = (BluetoothManager) (mContext.getSystemService(Context.BLUETOOTH_SERVICE));
        }
        if (mBluetoothAdvertiser != null) {
            mGattServer = mBluetoothManager.openGattServer(mContext, mMockServerCallBack);
            if (mGattServer == null) {
                LogUtil.d( "gattServer is null");
            }
        }
    }

    public void addService(BluetoothGattService service){
        if (mGattServer != null) {
            mGattServer.addService(service);
        }
    }

    public void clearServices(){
        if (mGattServer != null) {
            mGattServer.clearServices();
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void startAdvertise(AdvertiseSettings settings, AdvertiseData data,AdvertiseData scanResponse, AdvertiseCallback callback) {
        //获取BluetoothLeAdvertiser，BLE发送BLE广播用的一个API
        this.advertiseCallback=callback;
        if (mBluetoothAdvertiser != null) {
            try {
                mBluetoothAdvertiser.startAdvertising(settings,data,scanResponse,advertiseCallback);
            } catch (Exception e) {
                LogUtil.d( "Fail to setup BleService");
            }
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void stopAdvertise(){
        if(mBluetoothAdvertiser!=null && advertiseCallback!=null){
            mBluetoothAdvertiser.stopAdvertising(advertiseCallback);
        }
    }

    public BluetoothGattServer getmGattServer() {
        return mGattServer;
    }

    public void close(){
        mBluetoothAdvertiser=null;
        mGattServer=null;
        advertiseCallback=null;
        mMockServerCallBack=null;
    }
}
