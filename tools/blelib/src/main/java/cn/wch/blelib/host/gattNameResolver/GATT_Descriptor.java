package cn.wch.blelib.host.gattNameResolver;

import java.util.HashMap;
import java.util.UUID;

public class GATT_Descriptor {
    public static GATT_Descriptor gatt_descriptor;
    private static HashMap<String,String> map=new HashMap<>();
    static {
        map.put("00002900-0000-1000-8000-00805f9b34fb","Characteristic Extended Properties");
        map.put("00002901-0000-1000-8000-00805f9b34fb","Characteristic User Description");
        map.put("00002902-0000-1000-8000-00805f9b34fb","Client Characteristic Configuration");
        map.put("00002903-0000-1000-8000-00805f9b34fb","Server Characteristic Configuration");
        map.put("00002904-0000-1000-8000-00805f9b34fb","Characteristic Presentation Format");
        map.put("00002905-0000-1000-8000-00805f9b34fb","Characteristic Aggregate Format");
        map.put("00002906-0000-1000-8000-00805f9b34fb","Valid Range");
        map.put("00002907-0000-1000-8000-00805f9b34fb","External Report Reference");
        map.put("00002908-0000-1000-8000-00805f9b34fb","Report Reference");
        map.put("00002909-0000-1000-8000-00805f9b34fb","Number of Digitals");
        map.put("0000290a-0000-1000-8000-00805f9b34fb","Value Trigger Setting");
        map.put("0000290b-0000-1000-8000-00805f9b34fb","Environmental Sensing Configuration");
        map.put("0000290c-0000-1000-8000-00805f9b34fb","Environmental Sensing Measurement");
        map.put("0000290d-0000-1000-8000-00805f9b34fb","Environmental Sensing Trigger Setting");
        map.put("0000290e-0000-1000-8000-00805f9b34fb","Time Trigger Setting");
    }

    public static String getDescriptorType(UUID uuid){
        if(!map.containsKey(uuid.toString().toLowerCase())){
            return "Unknown Descriptor";
        }else {
            return map.get(uuid.toString().toLowerCase());
        }
    }

}
