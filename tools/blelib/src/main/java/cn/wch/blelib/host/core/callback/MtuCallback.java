package cn.wch.blelib.host.core.callback;

import android.bluetooth.BluetoothGatt;

public interface MtuCallback {
    void onMtuChanged(BluetoothGatt gatt, int mtu, int status);
}
