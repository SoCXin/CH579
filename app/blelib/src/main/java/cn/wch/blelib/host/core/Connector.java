package cn.wch.blelib.host.core;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import androidx.annotation.RequiresApi;

import cn.wch.blelib.host.core.callback.ConnectCallback;
import cn.wch.blelib.host.core.callback.DiscoverServiceCallback;
import cn.wch.blelib.host.core.callback.MtuCallback;
import cn.wch.blelib.host.core.callback.NotifyDataCallback;
import cn.wch.blelib.host.core.callback.ReadCallback;
import cn.wch.blelib.host.core.callback.RssiCallback;
import cn.wch.blelib.host.core.callback.WriteCallback;
import cn.wch.blelib.utils.FormatUtil;
import cn.wch.blelib.utils.LogUtil;

import static java.lang.Thread.sleep;

public class Connector {
    private Context context;
    private String mac;
    private ConnRuler connRuler;
    private ConnectCallback callback;
    private volatile boolean READ_NULL=false;
    private Handler handler = new Handler(Looper.getMainLooper());
    private Runnable runnable;
    private volatile boolean isRunnableAlive=false;
    private BluetoothGatt mBluetoothGatt;
    private enum STATE{
        BLE_CONNECTED,
        BLE_DISCONNECTED,
    }
    private STATE state= STATE.BLE_DISCONNECTED;
    private List<BluetoothGattService> mBluetoothGattServices=null;
    private  volatile boolean rwBusy=false;

    private  byte[] mRcvBuffer = new byte[20];
    private int max_packet=20;
    private int mtuOffset=3;
    private int mRcvLength = 0;

    private DiscoverServiceCallback discoverServiceCallback=null;
    private MtuCallback mtuCallback=null;
    private RssiCallback rssiCallback=null;
    private Map<String, NotifyDataCallback> NotifyMap;
    private Map<String, WriteCallback> WriteMap;
    private Map<String, ReadCallback> ReadMap;

    private BluetoothManager manager;

    public static final UUID CLIENT_CHARACTERISTIC_CONFIG = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    private enum  NOTIFY_STATE{
        PREPARE,
        OPENED,
        CLOSED,
        DEFAULT
    }

    private NOTIFY_STATE notifyState=NOTIFY_STATE.DEFAULT;

    public Connector(Context context, String label, ConnRuler connRuler, ConnectCallback callback) {
        this.context=context;
        this.mac = label;
        this.connRuler = connRuler;
        this.callback = callback;
        NotifyMap=new HashMap<>();
        WriteMap=new HashMap<>();
        ReadMap=new HashMap<>();
        manager= (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        startConnect();
    }

    public void setNewRuler(ConnRuler connRuler){
        this.connRuler = connRuler;
    }

    private void addRunnable() {
        removeRunable();
        isRunnableAlive=true;
        runnable = new Runnable() {
            @Override
            public void run() {
                isRunnableAlive=false;
                if(mBluetoothGatt!=null){
                    mBluetoothGatt.disconnect();
                    //2020.07.27
                    mBluetoothGatt.close();
                }
                if (callback != null) {
                    LogUtil.d("Connector "+mac+" ConnectTimeout");
                    callback.OnConnectTimeout(mac);
                }
            }
        };
        handler.postDelayed(runnable, connRuler.connectTimeout);
    }

    private void removeRunable() {
        isRunnableAlive=false;
        if (runnable != null) {
            handler.removeCallbacks(runnable);
            runnable = null;
        }
    }

    public void startConnect() {
        if(callback!=null){
            callback.OnConnecting(mac);
        }
        addRunnable();
        connect();
    }

    public void connect() {
        LogUtil.d(mac+"  connect");
        final BluetoothDevice device = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(mac);
        if (device == null) {
            LogUtil.d("Connector",mac,"Adapter getRemoteDevice ,result is null");
            return;
        }
        int connectionState = manager.getConnectionState(device,
                BluetoothProfile.GATT);
        if (connectionState == BluetoothProfile.STATE_DISCONNECTED) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                mBluetoothGatt = device.connectGatt(context, false, bluetoothGattCallback, BluetoothDevice.TRANSPORT_LE);
            } else {
                mBluetoothGatt = device.connectGatt(context, false, bluetoothGattCallback);
            }
        }else{
            LogUtil.d("Connector",mac,"State is not disconnected before connect");
            return;
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void setMtu(int mtu, MtuCallback mtuCallback){
        this.mtuCallback=null;
        this.mtuCallback=mtuCallback;
        if(mBluetoothGatt!=null){
            mBluetoothGatt.requestMtu(mtu);
        }
    }

    public void setRssiCallback(RssiCallback rssiCallback){
        this.rssiCallback=null;
        this.rssiCallback=rssiCallback;
    }


    private BluetoothGattCallback bluetoothGattCallback=new BluetoothGattCallback() {
        @Override
        public void onPhyUpdate(BluetoothGatt gatt, int txPhy, int rxPhy, int status) {
            super.onPhyUpdate(gatt, txPhy, rxPhy, status);
        }

        @Override
        public void onPhyRead(BluetoothGatt gatt, int txPhy, int rxPhy, int status) {
            super.onPhyRead(gatt, txPhy, rxPhy, status);
        }

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            BluetoothDevice device=gatt.getDevice();
            switch (newState){
                case BluetoothProfile.STATE_CONNECTING:
                    break;
                case BluetoothProfile.STATE_CONNECTED:
                    state= STATE.BLE_CONNECTED;
                    if(callback!=null){
                        callback.OnConnectSuccess(mac,new Connection(Connector.this,gatt));
                    }

                    if (Build.VERSION.SDK_INT >= 21) {
                        gatt.requestConnectionPriority(BluetoothGatt.CONNECTION_PRIORITY_HIGH);
                    }
                    if(connRuler.autoDiscoverServices){
                        handler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                boolean ret=discoverServices();
                                LogUtil.d("discoverServices-->"+ret);
                                //startDiscoverCountDown();
                            }
                        },connRuler.sleepTimeBeforeDiscover);
                    }else {
                        removeRunable();
                    }
                    break;
                case BluetoothProfile.STATE_DISCONNECTING:
                    break;
                case BluetoothProfile.STATE_DISCONNECTED:
                    LogUtil.d("DISCONNECT "+status);
                    gattClose();
                    if(isRunnableAlive){
                        LogUtil.d(" try -->reConnect");
                        connect();
                        return;
                    }
                    state= STATE.BLE_DISCONNECTED;
                    closeCallback();
                    if(callback!=null){
                        callback.OnDisconnect(mac,device,status);
                    }
                    break;
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            LogUtil.d("onServicesDiscovered-->"+status);
            removeRunable();
            cancelDiscoverCountDown();
            getSupportedServices();
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                boolean connPrio = mBluetoothGatt.requestConnectionPriority(BluetoothGatt.CONNECTION_PRIORITY_HIGH);
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            ReadCallback readCallback = ReadMap.get(characteristic.getUuid().toString());
            if(characteristic.getValue()==null || characteristic.getValue().length==0){
                LogUtil.d("READ NULL");
                if(readCallback!=null){
                    readCallback.OnReadNull(mac);
                }
                READ_NULL=true;
                rwBusy = false;
                return;
            }
            READ_NULL=false;
            LogUtil.d("onCharacteristicRead:"+ FormatUtil.bytesToHexString(characteristic.getValue()));
            if(readCallback !=null){
                readCallback.OnReadSuccess(mac,characteristic.getValue());
            }
            mRcvLength = characteristic.getValue().length;
            System.arraycopy(characteristic.getValue(), 0,
                    mRcvBuffer, 0, characteristic.getValue().length);
            rwBusy = false;

        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if(status!=BluetoothGatt.GATT_SUCCESS){
                LogUtil.d("write error: "+status);
                return;
            }
            rwBusy = false;
            WriteCallback writeCallback = WriteMap.get(characteristic.getUuid().toString());
            if(writeCallback!=null){
                writeCallback.OnWriteSuccess(mac,characteristic.getValue());
                LogUtil.d(mac+" onCharacteristicWrite:"+ FormatUtil.bytesToHexString(characteristic.getValue()));
            }else {
                return;
            }

        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            NotifyDataCallback notifyDataCallback = NotifyMap.get(characteristic.getUuid().toString());
            if(notifyDataCallback!=null){
                notifyDataCallback.OnData(mac,characteristic.getValue());
                LogUtil.d("onCharacteristicChanged:"+ FormatUtil.bytesToHexString(characteristic.getValue()));
            }else {
                return;
            }

        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorRead(gatt, descriptor, status);
        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            LogUtil.d("onDescriptorWrite--> "+"descriptor: "+descriptor.getUuid().toString()+" value: "+FormatUtil.bytesToHexString(descriptor.getValue())+" status: "+status);
            if(descriptor.getUuid().toString().equalsIgnoreCase(CLIENT_CHARACTERISTIC_CONFIG.toString())){
                if(status==BluetoothGatt.GATT_SUCCESS && (FormatUtil.bytesToHexString(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE ).equalsIgnoreCase(FormatUtil.bytesToHexString(descriptor.getValue())))){
                    //
                    notifyState=NOTIFY_STATE.OPENED;
                }else if(status==BluetoothGatt.GATT_SUCCESS && (FormatUtil.bytesToHexString(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE ).equalsIgnoreCase(FormatUtil.bytesToHexString(descriptor.getValue())))){
                    notifyState=NOTIFY_STATE.CLOSED;
                }else {
                    notifyState=NOTIFY_STATE.DEFAULT;
                }
            }

        }

        @Override
        public void onReliableWriteCompleted(BluetoothGatt gatt, int status) {
            super.onReliableWriteCompleted(gatt, status);
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            if(rssiCallback!=null){
                rssiCallback.onReadRemoteRssi(rssi, status);
            }
        }

        @Override
        public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
            max_packet=mtu-mtuOffset;
            mRcvBuffer = new byte[max_packet];
            if(mtuCallback!=null){
                mtuCallback.onMtuChanged(gatt, mtu, status);
            }
        }
    };

    public boolean isConnected(){
        if(state== STATE.BLE_CONNECTED){
            return true;
        }
        return false;
    }

    public boolean checkGatt(){
        return mBluetoothGatt==null? false:true;
    }

    public void setRwBusy(boolean rwBusy) {
        this.rwBusy = rwBusy;
    }

    public byte[] getmRcvBuffer() {
        return mRcvBuffer;
    }

    public int getmRcvLength() {
        return mRcvLength;
    }

    public String getMac(){
        return mac;
    }

    public boolean discoverServices(){
        if (mBluetoothGatt == null)
            return false;
        LogUtil.d("Connector",mac,"discoverServices");
        return mBluetoothGatt.discoverServices();
    }

    public void discoverServices(DiscoverServiceCallback d){
        this.discoverServiceCallback=d;
        if(!discoverServices()){
            d.OnError(new Throwable("Gatt is null"));
        }
    }
    private void getSupportedServices() {

        if(mBluetoothGatt != null) {
            mBluetoothGattServices = mBluetoothGatt.getServices();
            if(callback!=null){
                callback.OnDiscoverService(mac,mBluetoothGattServices);
            }
            if(discoverServiceCallback!=null){
                discoverServiceCallback.OnDiscover(mBluetoothGattServices);
            }
        }
    }

    public List<BluetoothGattService> getServiceList(){
        return mBluetoothGattServices;
    }

    private boolean waitIdle(long timeout) {
        long timeout_tmp = timeout *50+1;
        while (timeout_tmp-->0) {
            if (!rwBusy) {
                LogUtil.d("waitIdle break");
                return true;
            }
            try {
                sleep(0,1000*20);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        LogUtil.d("waitIdle Timeout!");
        rwBusy=false;
        return false;
    }

    public boolean syncReadSingle(BluetoothGattCharacteristic mReadCharacterist) {
        if (mBluetoothGatt==null || mReadCharacterist==null)
            return false;
        if(!isConnected()){
            return false;
        }
        setRwBusy(true);
        mBluetoothGatt.readCharacteristic(mReadCharacterist);
        return waitIdle(connRuler.readTimeout);
    }

    public boolean syncReadRepeat(BluetoothGattCharacteristic mReadCharacterist) {
        for(int i=0;i<connRuler.readNullRetryCount;i++){
            LogUtil.d("syncReadRepeat-->"+i);
            if(syncReadSingle(mReadCharacterist)){
                if(READ_NULL){
                    READ_NULL=false;
                }else{
                    READ_NULL=false;
                    return true;
                }
            }
        }
        return false;
    }

    public boolean syncWriteCharacteristic(BluetoothGattCharacteristic mWriteCharacterist, byte[] data) {
        LogUtil.d("try write :"+ FormatUtil.bytesToHexString(data));
        if (mBluetoothGatt==null || mWriteCharacterist==null)
            return false;
        if(!isConnected()){
            return false;
        };
        mWriteCharacterist.setValue(data);
        setRwBusy(true);
        boolean ok = mBluetoothGatt.writeCharacteristic(mWriteCharacterist);

        if (ok){
            LogUtil.d("wait...");
            ok = waitIdle(connRuler.writeTimeout);
        }
        LogUtil.d("waitIdle "+ok);
        return ok;
    }

    @Deprecated
    public void asyncWriteCharacteristic(BluetoothGattCharacteristic mWriteCharacterist, byte[] data, WriteCallback writeCallback) {
        if(writeCallback==null){
            return;
        }
        LogUtil.d("try write :"+ FormatUtil.bytesToHexString(data));
        if (mBluetoothGatt==null || mWriteCharacterist==null){
            writeCallback.OnError(mac,new Throwable("GATT or Character is null"));
            return ;
        }
        if(!isConnected()){
            writeCallback.OnError(mac,new Throwable("device is not connected"));
            return ;
        }
        WriteMap.put(mWriteCharacterist.getUuid().toString(),writeCallback);
        mWriteCharacterist.setValue(data);
        boolean ok = mBluetoothGatt.writeCharacteristic(mWriteCharacterist);
        if (!ok){
            LogUtil.d("write fail");
            writeCallback.OnWriteFail(mac,data);
        }
    }

    @Deprecated
    public boolean asyncWriteCharacteristic(BluetoothGattCharacteristic mWriteCharacterist, byte[] data) {
        LogUtil.d("try write :"+ FormatUtil.bytesToHexString(data));
        if (mBluetoothGatt==null || mWriteCharacterist==null){
            return false;
        }
        if(!isConnected()){
            return false;
        }
        mWriteCharacterist.setValue(data);
        return mBluetoothGatt.writeCharacteristic(mWriteCharacterist);
    }

    public void asyncReadCharacteristic(BluetoothGattCharacteristic mReadCharacterist,ReadCallback readCallback) {
        if(readCallback==null){
            return;
        }

        if (mBluetoothGatt==null || mReadCharacterist==null) {
            readCallback.OnError(mac,new Throwable("GATT or Character is null"));
            return;
        }
        if(!isConnected()){
            readCallback.OnError(mac,new Throwable("device is not connected"));
            return ;
        }
        ReadMap.put(mReadCharacterist.getUuid().toString(),readCallback);
        mBluetoothGatt.readCharacteristic(mReadCharacterist);

    }

    public  boolean isNotificationEnabled(
            BluetoothGattCharacteristic characteristic) {
        if (!checkGatt())
            return false;

        BluetoothGattDescriptor clientConfig = characteristic
                .getDescriptor(CLIENT_CHARACTERISTIC_CONFIG);
        if (clientConfig == null)
            return false;
        return clientConfig.getValue() == BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE;
    }

    public synchronized boolean enableNotify(boolean enable,BluetoothGattCharacteristic characteristic){
        LogUtil.d("try--->change notification "+enable);
        if(characteristic==null){
            return false;
        }
        if (!checkGatt())
            return false;
        if (mBluetoothGatt.setCharacteristicNotification(characteristic, enable)) {
            boolean ret=false;
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIG);
            if(descriptor != null) {

                byte[] val = enable ? BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE : BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE;
                descriptor.setValue(val);
                //初始化
                notifyState=NOTIFY_STATE.PREPARE;
                //开始写
                ret=mBluetoothGatt.writeDescriptor(descriptor);
                if(!ret){
                    notifyState=NOTIFY_STATE.DEFAULT;
                    LogUtil.d("writeDescriptor fail");
                    return false;
                }
                //等待onDescriptorWrite回调
                ret=false;
                int count=0;
                while (count<150){
                    if(enable && notifyState==NOTIFY_STATE.OPENED){
                        //打开通知成功
                        ret=true;
                        break;
                    }else if(!enable && notifyState==NOTIFY_STATE.CLOSED){
                        //关闭通知成功
                        ret=false;
                        break;
                    }

                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    count++;
                }
                notifyState=NOTIFY_STATE.DEFAULT;
            }
            LogUtil.d("Change Notify result: "+ret);
            return ret;
        }else{
            return false;
        }
    }

    public synchronized boolean setNotifyListener(BluetoothGattCharacteristic characteristic,NotifyDataCallback notifyDataCallback,boolean forceOpen){
        LogUtil.d("setNotifyListener: "+characteristic.getUuid().toString());
        if(!forceOpen){
            NotifyMap.put(characteristic.getUuid().toString(),notifyDataCallback);
            return true;
        }else {
            //强制打开通知
            if(!isNotificationEnabled(characteristic)){
                //当前未打开
                int retryCount=0;
                boolean ret=false;
                NotifyMap.put(characteristic.getUuid().toString(),notifyDataCallback);
                while (retryCount<10){
                    if(enableNotify(true,characteristic)){
                       ret=true;
                       break;
                    }
                    retryCount++;
                }
                if(!ret){
                    NotifyMap.remove(characteristic.getUuid().toString());
                    notifyDataCallback.OnError(mac,new Throwable("open notify function fail"));
                    return false;
                }
                return true;
            }else {
                LogUtil.d("notify has opened");
                NotifyMap.put(characteristic.getUuid().toString(),notifyDataCallback);
                return true;
            }
        }

    }

    public int getMax_packet(){
        return max_packet;
    }

    public int getMtu(){
        return max_packet+mtuOffset;
    }

    public void disconnect(){
        if(state== STATE.BLE_CONNECTED && checkGatt()){
            mBluetoothGatt.disconnect();
        }

    }

    public void closeCallback(){
        NotifyMap.clear();
        ReadMap.clear();
        WriteMap.clear();
    }

    public void close(){
        if(isRunnableAlive){
            removeRunable();
        }
        gattClose();
    }

    private void gattClose(){
        if(checkGatt()){
            mBluetoothGatt.disconnect();
            mBluetoothGatt.close();
            mBluetoothGatt=null;
        }
    }

    Runnable discoverRunnable;
    private void startDiscoverCountDown(){
        discoverRunnable=new Runnable() {
            @Override
            public void run() {
                boolean b = discoverServices();
                LogUtil.d("startDiscoverCountDown-->"+b);
            }
        };
        handler.postDelayed(discoverRunnable,500);

    }

    private void cancelDiscoverCountDown(){
        if(discoverRunnable!=null){
            handler.removeCallbacks(discoverRunnable);
        }
    }

}
