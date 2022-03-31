package cn.wch.ch579update.scan;

import android.Manifest;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;

import com.touchmcu.ui.DialogUtil;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.Toolbar;

import android.os.Handler;
import android.os.Looper;
import android.view.Menu;
import android.view.MenuItem;

import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import cn.wch.blelib.ch579.CH579BluetoothManager;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.Location;
import cn.wch.blelib.utils.LogUtil;
import cn.wch.ch579update.R;
import cn.wch.ch579update.scan.ui.DeviceAdapter;
import cn.wch.otaupdate.MainActivity;
import cn.wch.otaupdate.other.Constant;

public class ScanActivity extends ScanBaseActivity {

    RecyclerView recyclerView;
    DeviceAdapter adapter;
    boolean isScanning=false;
    Handler handler=new Handler(Looper.getMainLooper());
    @Override
    void initWidget() {
        setContentView(R.layout.activity_scan);
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle("搜索蓝牙设备");
        setSupportActionBar(toolbar);
        recyclerView=findViewById(R.id.rvDevice);

        adapter=new DeviceAdapter(this, new DeviceAdapter.Listener() {
            @Override
            public void onClick(BluetoothDevice device) {
                toAnotherActivity(device.getAddress());
            }
        });
        recyclerView.setLayoutManager(new LinearLayoutManager(this,LinearLayoutManager.VERTICAL,false));
        recyclerView.setAdapter(adapter);
        DividerItemDecoration dividerItemDecoration=new DividerItemDecoration(this,DividerItemDecoration.VERTICAL);
        recyclerView.addItemDecoration(dividerItemDecoration);
    }

    @Override
    protected void autoRun() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                startScan();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main,menu);
        menu.findItem(R.id.startScan).setVisible(!isScanning);
        menu.findItem(R.id.stopScan).setVisible(isScanning);
        menu.findItem(R.id.indicator).setVisible(isScanning);
        menu.findItem(R.id.indicator).setActionView(R.layout.menu_progress);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        int itemId = item.getItemId();
        if(itemId==R.id.startScan){
            startScan();
        }else if(itemId==R.id.stopScan){
            stopScan();
        }
        invalidateOptionsMenu();
        return super.onOptionsItemSelected(item);
    }

    void startScan(){
        LogUtil.d(Thread.currentThread().getId()+"");
        handler.post(new Runnable() {
            @Override
            public void run() {
                adapter.clear();
            }
        });

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            //大于安卓10，需要检查定位服务
            LogUtil.d("位置服务打开：" + Location.isLocationEnable(this));
            if (!Location.isLocationEnable(this)) {
                DialogUtil.getInstance().showSimpleDialog(this, "蓝牙扫描需要开启位置信息服务", new DialogUtil.IResult() {
                    @Override
                    public void onContinue() {
                        Location.requestLocationService(ScanActivity.this);
                    }

                    @Override
                    public void onCancel() {

                    }
                });

                return;
            }
        }

        if (!isBluetoothAdapterOpened()) {
            showToast("请先打开蓝牙");
            return;
        }

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            showToast("定位权限未开启");
            return;
        }
        isScanning=true;

        try {
            CH579BluetoothManager.getInstance().startScan(scanResult);
        } catch (BLELibException e) {
            e.printStackTrace();
        }

        invalidateOptionsMenu();
    }

    void stopScan(){
        isScanning=false;
        LogUtil.d("停止扫描");

        try {
            CH579BluetoothManager.getInstance().stopScan();
        } catch (BLELibException e) {
            e.printStackTrace();
        }
        invalidateOptionsMenu();
    }

    void toAnotherActivity(String mac){
        stopScan();
        Intent intent=new Intent(this, MainActivity.class);
        intent.putExtra(Constant.ADDRESS,mac);
        startActivity(intent);
    }


    cn.wch.blelib.ch579.callback.ScanResult scanResult=new cn.wch.blelib.ch579.callback.ScanResult() {
        @Override
        public void onResult(BluetoothDevice device, int rssi, byte[] broadcastRecord) {
            LogUtil.d(device.getAddress());
            if(adapter!=null && device.getType()==BluetoothDevice.DEVICE_TYPE_LE){
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        adapter.update(device,rssi);
                    }
                });

            }
        }
    };

}