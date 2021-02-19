package cn.wch.blelib.ch579.callback;

/**
 * 连接状态回调接口
 */
public interface ConnectStatus {

    /**
     * 连接失败
     * @param t
     */
    void OnError(Throwable t);

    /**
     * 开始连接
     */
    void OnConnecting();

    /**
     * 连接成功
     */
    void OnConnectSuccess(String mac);



    /**
     * 该设备不是CH9141
     */
    void onInvalidDevice(String mac);

    /**
     * 连接超时
     */
    void OnConnectTimeout(String mac);

    /**
     * 连接被断开
     * @param status
     */
    void OnDisconnect(String mac, int status);
}
