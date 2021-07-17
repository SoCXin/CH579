package cn.wch.blelib.host.advertise.bean;

public class ManufactureBean {
    int id;
    String data;

    public ManufactureBean(int id, String data) {
        this.id = id;
        this.data = data;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getData() {
        return data;
    }

    public void setData(String data) {
        this.data = data;
    }

    @Override
    public String toString() {
        return "ManufacturBean{" +
                "id=" + id +
                ", data='" + data + '\'' +
                '}';
    }
}
