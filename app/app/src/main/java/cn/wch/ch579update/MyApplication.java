package cn.wch.ch579update;

import android.app.Application;

import cn.wch.blelib.ch579.CH579BluetoothManager;
import cn.wch.blelib.ch579.ota.CH579OTAManager;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.LogUtil;

public class MyApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        try {
            CH579BluetoothManager.getInstance().init(this);
            CH579OTAManager.getInstance().init(this);

        } catch (BLELibException e) {
            LogUtil.d(e.getMessage());
            e.printStackTrace();
        }

    }
}
