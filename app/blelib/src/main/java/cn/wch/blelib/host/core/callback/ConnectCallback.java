package cn.wch.blelib.host.core.callback;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattService;

import java.util.List;

import cn.wch.blelib.host.core.Connection;

public interface ConnectCallback {

    /**
     * 连接失败
     * @param mac
     * @param t
     */
    void OnError(String mac, Throwable t);

    /**
     * 开始连接
     * @param mac
     */
    void OnConnecting(String mac);

    /**
     * 连接成功
     * @param mac
     * @param connection
     */
    void OnConnectSuccess(String mac, Connection connection);

    /**
     * 连接成功后才能枚举服务
     * @param mac
     * @param list
     */
    void OnDiscoverService(String mac, List<BluetoothGattService> list);

    /**
     * 连接超时
     * @param mac
     */
    void OnConnectTimeout(String mac);

    /**
     * 连接被断开
     * @param mac
     * @param bluetoothDevice
     * @param status
     */
    void OnDisconnect(String mac, BluetoothDevice bluetoothDevice, int status);
}
