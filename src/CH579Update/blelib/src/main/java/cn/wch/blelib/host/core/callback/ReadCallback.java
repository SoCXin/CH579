package cn.wch.blelib.host.core.callback;

public interface ReadCallback {
    void OnError(String mac, Throwable t);
    void OnReadSuccess(String mac, byte[] data);
    void OnReadNull(String mac);
}
