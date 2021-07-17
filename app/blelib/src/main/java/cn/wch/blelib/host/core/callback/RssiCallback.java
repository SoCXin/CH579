package cn.wch.blelib.host.core.callback;

public interface RssiCallback {
    void onReadRemoteRssi(int rssi, int status);
}
