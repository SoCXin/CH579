package cn.wch.blelib.utils;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import androidx.annotation.NonNull;


public class BLEUtil {
    public static boolean isSupportBLE(Context context){
        return Build.VERSION.SDK_INT>=Build.VERSION_CODES.KITKAT &&
                (context.getApplicationContext().getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE));
    }

    public static boolean isAdapterEnable( BluetoothAdapter bluetoothAdapter){
        if(bluetoothAdapter==null) {
            throw new NullPointerException("bluetoothAdapter is null");
        }
        return bluetoothAdapter.isEnabled();
    }

    public static boolean isValidMac(String macStr) {
        if (macStr == null || macStr.equals("")) {
            return false;
        }
        String macAddressRule = "([A-Fa-f0-9]{2}[-,:]){5}[A-Fa-f0-9]{2}";
        // 这是真正的MAC地址；正则表达式；
        if (macStr.matches(macAddressRule)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * 根据需求寻找目标Characteristic
     * @param bluetoothGattServices
     */
    public static BluetoothGattCharacteristic getCharacterist(List<BluetoothGattService> bluetoothGattServices, String ServiceUUID,String CharacterUUID){
        boolean ok=false;
        BluetoothGattCharacteristic c=null;
        if(bluetoothGattServices==null || bluetoothGattServices.size()==0){
            ok=false;
            return null;
        }
        for (BluetoothGattService service:bluetoothGattServices) {
            if(service.getUuid().toString().equals(ServiceUUID)){
                for (BluetoothGattCharacteristic characteristic:service.getCharacteristics()) {
                    String s=characteristic.getUuid().toString()+" ";
                    if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_READ)>0){
                        s+="READ ";
                    }
                    if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE)>0){
                        s+="WRITE ";
                    }
                    if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY)>0){
                        s+="NOTIFY ";
                    }
                    if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_INDICATE)>0){
                        s+="INDICATE ";
                    }
                    if(characteristic.getUuid().toString().equals(CharacterUUID)){
                        ok=true;
                        c=characteristic;
                    }
                }
            }

        }
        return c;
    }

    public static String getProperty(BluetoothGattCharacteristic characteristic){
        String s="";
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_READ)>0){
            s+="READ ";
        }
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE)>0){
            s+="WRITE ";
        }
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE)>0){
            s+="WRITE NO RESPONSE ";
        }
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_NOTIFY)>0){
            s+="NOTIFY ";
        }
        if((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_INDICATE)>0){
            s+="INDICATE ";
        }
        return  s;
    }

    public static ArrayList<BluetoothDevice> getBondedDevices(@NonNull BluetoothAdapter bluetoothAdapter){
        return new ArrayList<>(bluetoothAdapter.getBondedDevices());
    }

    public static boolean isBondedDevice(@NonNull BluetoothAdapter bluetoothAdapter,@NonNull String mac){
        ArrayList<BluetoothDevice> b=getBondedDevices(bluetoothAdapter);
        for (BluetoothDevice d:b) {
            if(d.getAddress().equals(mac)){
                return true;
            }
        }
        return false;
    }

    public static String getDeviceType(@NonNull BluetoothDevice device){
        int type=device.getType();
        switch (type){
            case BluetoothDevice.DEVICE_TYPE_UNKNOWN:
                return "Unknown";
            case BluetoothDevice.DEVICE_TYPE_CLASSIC:
                return "BR/EDR devices";
            case BluetoothDevice.DEVICE_TYPE_LE:
                return "LE-only";
            case BluetoothDevice.DEVICE_TYPE_DUAL:
                return "BR/EDR/LE";
            default:
                return "";
        }
    }

    public static boolean removeBond(BluetoothDevice device){
        boolean returnValue=false;
        try {
            Method removeBondMethod = BluetoothDevice.class.getDeclaredMethod("removeBond");
            returnValue=(boolean) removeBondMethod.invoke(device);

        } catch (NoSuchMethodException e) {
            LogUtil.d("BluetoothDevice.removeBond method not found");
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return returnValue;
    }

    public static boolean createBond(BluetoothDevice device){
        boolean returnValue=false;
        try {
            Method createBondMethod = BluetoothDevice.class.getDeclaredMethod("createBond");
            returnValue=(boolean) createBondMethod.invoke(device);
        } catch (NoSuchMethodException e) {
            LogUtil.d("BluetoothDevice.createBondMethod method not found");
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
        return returnValue;
    }

    public static void printAllHidden(Class clsShow) {
        try {
            // 取得所有方法
            Method[] hideMethod = clsShow.getMethods();
            int i = 0;
            for (; i < hideMethod.length; i++) {
                LogUtil.d("method name"+hideMethod[i].getName());
            }
            // 取得所有常量
            Field[] allFields = clsShow.getFields();
            for (i = 0; i < allFields.length; i++) {
                LogUtil.d("Field name"+allFields[i].getName());
            }
        } catch (SecurityException e) {
            // throw new RuntimeException(e.getMessage());
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            // throw new RuntimeException(e.getMessage());
            e.printStackTrace();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

}
