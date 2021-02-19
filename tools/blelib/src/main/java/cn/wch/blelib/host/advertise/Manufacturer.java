package cn.wch.blelib.host.advertise;

import java.util.HashMap;

import cn.wch.blelib.utils.LogUtil;


/**
 * see: https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
 */
class Manufacturer {
    private static HashMap<Integer, String> ManufacturerMap = new HashMap<Integer, String>();
    static public String getManufacturer(final int u)
    {
        String result = ManufacturerMap.get(u);
        if(result == null) {
            result = "Unknown("+String.format("0x%04X",u)+")";
        }
        LogUtil.d("company name: "+u+" "+result);
        return result;
    }
    static {
        ManufacturerMap.put(0x0002,"Intel Corp.(0x0002)");
        ManufacturerMap.put(0x0006,"Microsoft(0x0006)");
        ManufacturerMap.put(0x000D,"Texas Instruments Inc.(0x000D)");
        ManufacturerMap.put(0x004C,"Apple, Inc.(0x004C)");
        ManufacturerMap.put(0x0059,"Nordic Semiconductor ASA(0x0059)");
        ManufacturerMap.put(0x00D2,"Dialog Semiconductor B.V.(0x00D2)");
        ManufacturerMap.put(0x027D,"HUAWEI Technologies Co., Ltd.(0x027D)");
        ManufacturerMap.put(0x038F,"Xiaomi Inc.(0x038F)");
    }
}
