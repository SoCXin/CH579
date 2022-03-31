package cn.wch.blelib.host.advertise.bean;

public class AdvBean {
    private int len;
    private byte type;
    private String data;

    public AdvBean(int len, byte type, String data) {
        this.len = len;
        this.type = type;
        this.data = data;
    }

    public int getLen() {
        return len;
    }

    public void setLen(int len) {
        this.len = len;
    }

    public byte getType() {
        return type;
    }

    public void setType(byte type) {
        this.type = type;
    }

    public String getData() {
        return data;
    }

    public void setData(String data) {
        this.data = data;
    }

    @Override
    public String toString() {
        return "AdvBean{" +
                "len=" + len +
                ", type=" + type +
                ", data='" + data + '\'' +
                '}';
    }
}
