package cn.wch.blelib.host.scan;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanSettings;
import android.os.Build;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.annotation.RequiresPermission;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.FormatUtil;
import cn.wch.blelib.utils.LogUtil;


public class BLEScanUtil {

    private static BLEScanUtil mThis;
    private BluetoothAdapter.LeScanCallback leScanCallback;
    private Map<String,String> map;

    public static BLEScanUtil getInstance(){
        if(mThis==null){
            synchronized (BLEScanUtil.class){
                mThis=new BLEScanUtil();
            }
        }
        return mThis;
    }

    public BLEScanUtil() {
        map=new HashMap<>();
    }

    /**
     * 开始扫描，需要过滤条件
     * @param scanRuler
     * @param observer
     */
    @RequiresPermission(allOf = {Manifest.permission.ACCESS_FINE_LOCATION})
    public synchronized void startScan(final ScanRuler scanRuler, final ScanObserver observer) throws BLELibException {
        if(BluetoothAdapter.getDefaultAdapter()==null || !BluetoothAdapter.getDefaultAdapter().isEnabled()){
            throw new BLELibException("BluetoothAdapter should be opened");
        }
        if(observer==null ){
            throw new BLELibException("observer is null");
        }
        leScanCallback=new BluetoothAdapter.LeScanCallback() {
            @Override
            public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
                if(matchRuler(device,rssi,scanRecord,scanRuler)){
                    observer.OnScanDevice(device,rssi,scanRecord);
                }
            }
        };
        BluetoothAdapter.getDefaultAdapter().startLeScan(leScanCallback);
    }

    /**
     * 开始扫描,不过滤
     * @param scanCallback
     */
    @RequiresPermission(allOf = {Manifest.permission.ACCESS_FINE_LOCATION})
    public synchronized void startScan(BluetoothAdapter.LeScanCallback scanCallback) throws BLELibException {
        if(BluetoothAdapter.getDefaultAdapter()==null || !BluetoothAdapter.getDefaultAdapter().isEnabled()){
            throw new BLELibException("BluetoothAdapter should be opened");
        }
        if(scanCallback==null ){
            throw new BLELibException("scanCallback is null");
        }
        this.leScanCallback=scanCallback;
        BluetoothAdapter.getDefaultAdapter().startLeScan(leScanCallback);
    }


    /**
     * 开始扫描，不会返回重复的设备
     * @param scanRuler
     * @param observer
     */
    @RequiresPermission(allOf = {Manifest.permission.ACCESS_FINE_LOCATION})
    public synchronized void startScanNoRepeat(final ScanRuler scanRuler, final ScanObserver observer) throws BLELibException {
        if(BluetoothAdapter.getDefaultAdapter()==null || !BluetoothAdapter.getDefaultAdapter().isEnabled()){
            throw new BLELibException("BluetoothAdapter should be opened");
        }
        if(observer==null ){
            throw new BLELibException("observer is null");
        }
        leScanCallback=new BluetoothAdapter.LeScanCallback() {
            @Override
            public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
                if(matchRuler(device,rssi,scanRecord,scanRuler)){
                    if(map.containsKey(device.getAddress())){
                        return;
                    }
                    map.put(device.getAddress(),"");
                    observer.OnScanDevice(device,rssi,scanRecord);
                }
            }
        };
        BluetoothAdapter.getDefaultAdapter().startLeScan(leScanCallback);
    }

    /**
     * 停止扫描
     */
    public synchronized void stopScan(){
        map.clear();
        if(leScanCallback!=null && BluetoothAdapter.getDefaultAdapter()!=null){
            BluetoothAdapter.getDefaultAdapter().stopLeScan(leScanCallback);
        }
    }

    public static synchronized boolean matchRuler(BluetoothDevice device, int rssi, byte[] scanRecord, ScanRuler scanRuler){

        if(scanRuler.union){//取条件并集

            if (scanRuler.Name != null) {
                boolean contain = false;
                ArrayList<String> Names = new ArrayList<>(Arrays.asList(scanRuler.Name));
                for (String name : Names) {
                    if ((device.getName() == null && name == null) || (device.getName() != null && device.getName().equals(name))) {
                        contain=true;
                    }
                }
                if(contain){
                    return true;
                }
            }

            if (scanRuler.MAC != null) {
                boolean contain = false;
                ArrayList<String> MAC = new ArrayList<>(Arrays.asList(scanRuler.MAC));
                for (String mac : MAC) {
                    if (device.getAddress() != null && device.getAddress().equals(mac)) {
                        contain=true;
                    }
                }
                if(contain){
                    return true;
                }
            }

            if (scanRuler.scanRecord != null) {
                boolean contain = false;
                ArrayList<byte[]> records = scanRuler.scanRecord;
                for (byte[] b : records) {
                    if (FormatUtil.bytesToHexString(scanRecord).contains(FormatUtil.bytesToHexString(b))) {


                        contain = true;
                    }
                }
                if (contain) {
                    return true;
                }
            }

            if(scanRuler.rssiMin>0 || scanRuler.rssiMax>0){
                //未添加rssi过滤

            }else {
                if (rssi >= scanRuler.rssiMin && rssi <= scanRuler.rssiMax) {
                    return true;
                }
            }

            return false;
        }else{//条件取交集
            if (scanRuler.Name != null) {
                boolean contain=false;
                ArrayList<String> Names = new ArrayList<>(Arrays.asList(scanRuler.Name));
                for (String name : Names) {
                    if ((device.getName() == null && name == null) ) {
                        contain=true;
                        break;
                    }
                    if (name!=null && (device.getName() != null && device.getName().toUpperCase().contains(name.toUpperCase()))) {
                        contain=true;
                        break;
                    }
                }
                if(!contain){
                    return false;
                }
            }

            if (scanRuler.MAC != null) {
                boolean contain=false;
                ArrayList<String> MAC = new ArrayList<>(Arrays.asList(scanRuler.MAC));
                for (String mac : MAC) {
                    if (device.getAddress() != null && device.getAddress().toUpperCase().contains(mac.toUpperCase())) {
                        contain=true;
                    }
                }
                if(!contain){
                    return false;
                }
            }
            if (scanRuler.scanRecord != null) {
                boolean contain = false;
                ArrayList<byte[]> records = scanRuler.scanRecord;
                for (byte[] b : records) {

                    if (FormatUtil.bytesToHexString(scanRecord).toUpperCase().replace(" ","").contains(FormatUtil.bytesToHexString(b).toUpperCase().replace(" ",""))) {
                        contain = true;
                        break;
                    }
                }
                if (!contain) {
                    return false;
                }
            }
            if(scanRuler.rssiMin>0 || scanRuler.rssiMax>0){
                //未添加rssi过滤
            }else {
                if (rssi <= scanRuler.rssiMin || rssi >= scanRuler.rssiMax) {
                    return false;
                }
            }
            return true;
        }
    }

}
