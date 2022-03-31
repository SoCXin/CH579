package cn.wch.blelib.host.scan;

import android.bluetooth.BluetoothDevice;

public interface ScanObserver {
    void OnScanDevice(BluetoothDevice bluetoothDevice, int rssi, byte[] broadcastRecord);
}
