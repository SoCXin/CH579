package cn.wch.blelib.host.core;

import android.Manifest;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.os.Build;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.List;
import java.util.Random;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.annotation.RequiresPermission;

import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.host.core.callback.DiscoverServiceCallback;
import cn.wch.blelib.host.core.callback.MtuCallback;
import cn.wch.blelib.host.core.callback.NotifyDataCallback;
import cn.wch.blelib.host.core.callback.ReadCallback;
import cn.wch.blelib.host.core.callback.RssiCallback;
import cn.wch.blelib.host.core.callback.WriteCallback;
import cn.wch.blelib.utils.LogUtil;

import static java.lang.Thread.sleep;

public class Connection {
    public Connector connector;
    public BluetoothGatt bluetoothGatt;
    public String MAC;



    public Connection(@NonNull Connector connector, @NonNull BluetoothGatt bluetoothGatt) {
        this.connector = connector;
        this.bluetoothGatt = bluetoothGatt;
        MAC=connector.getMac();
    }


    private Connection setNewRuler(@NonNull ConnRuler connRuler){
        connector.setNewRuler(connRuler);
        return this;
    }

    /**
     * 设置mtu
     * @param mtu
     * @param mtuCallback
     */
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void setMtu(int mtu, MtuCallback mtuCallback){
        if(connector==null){
            return ;
        }
        connector.setMtu(mtu, mtuCallback);
    }

    public int getMtu()throws BLELibException {
        if(connector==null){
            throw new BLELibException("connector is null");
        }
        return connector.getMtu();
    }


    /**
     * 写数据，从数组的0开始，写长度为length的数据。数据超过最大包长会自动分包
     * @param characteristic
     * @param data
     * @param length
     * @return
     */
    public int write(@NonNull BluetoothGattCharacteristic characteristic, @NonNull byte[] data,int length){
       int total=0;
        if(connector==null || characteristic==null || data==null || length<0){
            return -1;
        }
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE)==0 && (characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE)==0){
            return -2;
        }
        if(data.length==0 || length==0){
            return 0;
        }
        int packetLen=connector.getMax_packet();
        int fullCount=Math.min(length,data.length)/packetLen;
        for(int i=0;i<fullCount;i++){
            byte[] tmp=new byte[packetLen];
            System.arraycopy(data,i*packetLen,tmp,0,packetLen);
            if(!connector.syncWriteCharacteristic(characteristic,tmp)) {
                return total;
            }
            total+=tmp.length;
            if(i==(fullCount-1) && data.length%packetLen==0){
                break;
            }
        }
        byte[] tmp=new byte[Math.min(length,data.length)%packetLen];
        if(tmp.length!=0) {
            System.arraycopy(data, fullCount * packetLen, tmp, 0, tmp.length);
            if (!connector.syncWriteCharacteristic(characteristic, tmp)) {
                return total;
            }
            total+=tmp.length;
            LogUtil.d("final write "+tmp.length);
        }
        return total;
    }


    /**
     * 将数据填充到一包中发送
     * @param characteristic
     * @param data
     * @param length
     * @return
     */
    public boolean writeSingle(@NonNull BluetoothGattCharacteristic characteristic, @NonNull byte[] data,int length){
        if(connector==null || characteristic==null || data==null || length<0){
            return false;
        }
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE)==0 && (characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE)==0){
            return false;
        }
        if(data.length==0 || length==0){
            return true;
        }
        byte[] tmp=new byte[Math.min(length,data.length)];
        System.arraycopy(data,0,tmp,0,tmp.length);
        return connector.syncWriteCharacteristic(characteristic,tmp);
    }



    /**
     * 从特征值读取数据
     * @param characteristic 特征值
     * @param single 是否多次尝试读取到
     * @return 读取的数据
     */
    public byte[] read(@NonNull BluetoothGattCharacteristic characteristic,boolean single){
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_READ)==0){
            return null;
        }
        if(single? connector.syncReadSingle(characteristic): connector.syncReadRepeat(characteristic)){
            byte[] data = connector.getmRcvBuffer();
            int length = connector.getmRcvLength();
            byte[] p1 = new byte[length];
            System.arraycopy(data, 0, p1, 0, length);
            return p1;
        }else{
            return null;
        }
    }

    /**
     * 异步读取数据
     * @param characteristic
     * @param readCallback
     */
    public void read(@NonNull BluetoothGattCharacteristic characteristic, ReadCallback readCallback){
        connector.asyncReadCharacteristic(characteristic,readCallback);
    }

    /**
     *
     * @param characteristic
     * @param notifyDataCallback
     * @param forceOpen
     */
    public boolean setNotifyListener(@NonNull BluetoothGattCharacteristic characteristic, NotifyDataCallback notifyDataCallback, boolean forceOpen){
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY)>0) {
            return connector.setNotifyListener(characteristic, notifyDataCallback,forceOpen);
        }else {
            notifyDataCallback.OnError(MAC,new Throwable("this characteristic do not has NOTIFY Property"));
        }
        return false;
    }

    public boolean enableNotify(boolean enable,@NonNull BluetoothGattCharacteristic characteristic){

        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY)>0) {
            return connector.enableNotify(enable, characteristic);
        }else {
            return false;
        }
    }

    public boolean getNotifyState(@NonNull BluetoothGattCharacteristic characteristic){
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY)>0){
            return connector.isNotificationEnabled(characteristic);
        }else {
            return false;
        }
    }


    /**
     * 向特征值写数据后再从特征值读取数据
     * @param mWriteCharacter 写数据的特征值
     * @param writeData 要写的数据
     * @param mReadCharacter 读数据的特征值
     * @return 读取的数据
     */
    public byte[] spWRCharacteristic(@NonNull BluetoothGattCharacteristic mWriteCharacter,@NonNull byte[] writeData,@NonNull BluetoothGattCharacteristic mReadCharacter,long waitTimeBeforeRead){

        if(write(mWriteCharacter,writeData,writeData.length)==writeData.length){
            if(waitTimeBeforeRead>0){
                try {
                    sleep(waitTimeBeforeRead);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            return read(mReadCharacter,false);
        }
        return null;
    }

    /**
     * 向特征值写数据后再从特征值读取数据
     * @param mWriteCharacter 写数据的特征值
     * @param writeData 要写的数据
     * @param mReadCharacter 读数据的特征值
     * @param readNum 想读取的数据的长度
     * @return 读取的数据
     */
    public byte[] spWRCharacteristic(@NonNull BluetoothGattCharacteristic mWriteCharacter,@NonNull byte[] writeData,@NonNull BluetoothGattCharacteristic mReadCharacter,int readNum,long waitTimeBeforeRead){
        if(write(mWriteCharacter,writeData,writeData.length)==writeData.length && readNum>=0){
            if(waitTimeBeforeRead>0){
                try {
                    sleep(waitTimeBeforeRead);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            int offset=0;
            byte[] buffer=new byte[readNum];
            while (offset<readNum){
                byte[] bytes = read(mReadCharacter, false);
                if(bytes==null || bytes.length==0){
                    return null;
                }
                int num=(bytes.length+offset>readNum) ? (readNum-offset) : bytes.length;
                System.arraycopy(bytes,0,buffer,offset,num);
                offset+=num;
            }
            return buffer;

        }
        return null;
    }

    /**
     * 是否处于连接状态
     * @return
     */
    public boolean isConnected(){
        return  connector.isConnected();
    }

    /**
     * 断开连接
     */
    public void disconnect(){
        connector.disconnect();
    }

    /**
     * 关闭资源
     */
    public void close(){
        connector.close();
    }


    public boolean readRssi(){
        if(bluetoothGatt!=null){
            return bluetoothGatt.readRemoteRssi();
        }
        return false;
    }

    public void setRSSICallback(RssiCallback rssiCallback){
        if(bluetoothGatt==null || connector==null){
            return;
        }
        connector.setRssiCallback(rssiCallback);
    }

    public Connector getConnector() {
        return connector;
    }
}
