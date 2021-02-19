package cn.wch.blelib.host.advertise;

import android.util.Base64;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import cn.wch.blelib.host.advertise.bean.AdvBean;
import cn.wch.blelib.host.advertise.bean.ManufactureBean;
import cn.wch.blelib.utils.LogUtil;

public class AdvertisingParse {
    /**
     * 解析广播包
     * @param adv 广播包数据
     * @return
     */
    public static Map<String,Object> parse(byte[] adv){
        if(adv==null || adv.length==0){
            return null;
        }
        Map<String ,Object> map=new HashMap<>();
        List<AdvBean> jsonArray=new ArrayList<AdvBean>();
        int off=0;

        map.put("Flag", "");


        map.put("Manufacturer", null);

        while(off<adv.length){
            int len=adv[off] & 0xff;
            if(len==0){
                break;
            }
            byte type=adv[off+1];
            byte[] data=new byte[len-1];
            System.arraycopy(adv,off+2,data,0,data.length);
            off+=(len+1);
            if(type == (byte)0x01 && data.length==1) {

                map.put("Flag", AdvertiseUtil.parseFlag(data[0]));


            }
            if(type == (byte)0xff){
                if(data.length<2){
                    LogUtil.d("AdvertisingParse Manufacturer invalid");
                }else {
                    int id = data[0] & 0xff | ((data[1] & 0x00ff) <<8);
                    byte[] mdata = new byte[data.length - 2];
                    System.arraycopy(data, 2, mdata, 0, mdata.length);
                    ManufactureBean manufactureBean = new ManufactureBean(id, Base64.encodeToString(mdata, Base64.DEFAULT));

                    map.put("Manufacturer", manufactureBean);

                }
            }
            {
                AdvBean advBean=new AdvBean(len,type,Base64.encodeToString(data,Base64.DEFAULT));
                jsonArray.add(advBean);
            }
        }

        map.put("Detail",jsonArray);

        return map;
    }
}
