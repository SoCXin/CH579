package cn.wch.blelib.host.scan;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.annotation.RequiresPermission;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.LogUtil;

@RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
public class BLEScanUtil2 {
    private static BLEScanUtil2 bleScanUtil2;
    private BluetoothLeScanner scanner;
    private List<ScanFilter> scanFilters;
    private ScanSettings scanSettings;

    private ScanCallback scanCallback;


    public static BLEScanUtil2 getInstance(Context context){
        if(bleScanUtil2 ==null){
            synchronized (BLEScanUtil2.class){
                bleScanUtil2 =new BLEScanUtil2(context);
            }
        }
        return bleScanUtil2;
    }
    public BLEScanUtil2(Context context) {
        scanFilters=new ArrayList<>();
    }

    /**
     * 示例
     */
   /* public void initScanPolicy(){
        ScanFilter filter=new ScanFilter.Builder().setManufacturerData(0xD90C,new byte[]{0x00,0x00,0x00,0x00,0x00,0x00}).build();
        scanSettings=new ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_BALANCED).setCallbackType(ScanSettings.CALLBACK_TYPE_FIRST_MATCH).build();
        scanFilters.add(filter);
    }*/

    /**
     * 初始化搜索过滤条件
     * @param settings
     * @param filters
     */
    public void initScanPolicy(@NonNull ScanSettings settings, @NonNull ScanFilter... filters){
        scanSettings=settings;
        scanFilters.clear();
        scanFilters.addAll(Arrays.asList(filters));
    }


    /**
     * 开始扫描
     * 需要打开蓝牙，需要ACCESS_FINE_LOCATION权限
     * @param callback 结果回调
     */
    @RequiresPermission(anyOf = {Manifest.permission.ACCESS_FINE_LOCATION,Manifest.permission.ACCESS_COARSE_LOCATION})
    public void startLeScan(@NonNull ScanCallback callback) throws BLELibException {
        if(BluetoothAdapter.getDefaultAdapter()==null || !BluetoothAdapter.getDefaultAdapter().isEnabled()){
            throw new BLELibException("BluetoothAdapter should be opened");
        }
        scanner=BluetoothAdapter.getDefaultAdapter().getBluetoothLeScanner();
        if(scanner!=null && scanSettings!=null && scanFilters!=null){
            LogUtil.d("-->startLeScan");
            this.scanCallback=callback;
            scanner.startScan(scanFilters,scanSettings,callback);
        }else {
            LogUtil.d("scanner or scanSettings or scanFilters is null,may be invoke init() first");
        }
    }

    /**
     * 停止扫描
     */
    public void stopLeScan(){
        if(BluetoothAdapter.getDefaultAdapter()==null || !BluetoothAdapter.getDefaultAdapter().isEnabled()){
            LogUtil.d("BluetoothAdapter is closed");
            return;
        }
        scanner=BluetoothAdapter.getDefaultAdapter().getBluetoothLeScanner();
        if(scanner!=null && scanCallback!=null){
            LogUtil.d("-->stopLeScan");
            scanner.stopScan(scanCallback);
        }
        scanCallback=null;
    }


    /**
     * 验检查该设备是否支持BLE
     * @param context
     * @return
     */
    public static boolean isSupportBle(Context context){
        PackageManager packageManager=context.getPackageManager();
        return BluetoothAdapter.getDefaultAdapter()!=null && packageManager!=null && packageManager.hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE);
    }

    /**
     *检查该mac地址是否合法
     * @param address
     * @return
     */
    public static boolean isAddressValid(String address){
        return BluetoothAdapter.checkBluetoothAddress(address);
    }



}
