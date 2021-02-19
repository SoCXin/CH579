package cn.wch.blelib.host.core.callback;

public interface NotifyDataCallback {
    void OnError(String mac, Throwable t);
    void OnData(String mac, byte[] data);
}
