package cn.wch.blelib.host.core.callback;

import android.bluetooth.BluetoothGattService;

import java.util.List;

public interface DiscoverServiceCallback {
    void OnDiscover(List<BluetoothGattService> list);
    void OnError(Throwable t);
}
