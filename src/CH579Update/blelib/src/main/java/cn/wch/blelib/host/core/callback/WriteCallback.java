package cn.wch.blelib.host.core.callback;

public interface WriteCallback {
    void OnWriteFail(String mac, byte[] writeData);
    void OnWriteSuccess(String mac, byte[] writeData);
    void OnWriteEnd(String mac);
    void OnError(String mac, Throwable t);
}
