package cn.wch.blelib.host.advertise;

public class AdvertiseUtil {
    public static String parseFlag(byte flag){
        String s="";
        if((flag & 0x01) != 0x00){
            s+="LE Limited Discoverable; ";
        }
        if((flag & 0x02) != 0x00){
            s+="LE General Discoverable; ";
        }
        if((flag & 0x04) != 0x00){
            s+="BR/EDR Not Supported; ";
        }else {
            s+="BR/EDR Supported; ";
        }
        if((flag & 0x08) != 0x00){
            s+="LE and BR/EDR Controller;";
        }
        if((flag & 0x10) != 0x00){
            s+="LE and BR/EDR Host;";
        }
        return s;
    }

    public static String getCompanyName(int id){
        return Manufacturer.getManufacturer(id);
    }
}
