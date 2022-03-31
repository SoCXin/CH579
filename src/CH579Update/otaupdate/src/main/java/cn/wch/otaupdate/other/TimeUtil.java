package cn.wch.otaupdate.other;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class TimeUtil {

    private static SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss:SSS", Locale.getDefault());
    private static SimpleDateFormat simpleFormat=new SimpleDateFormat("yyyy-MM-dd",Locale.getDefault());
    private static SimpleDateFormat fileFormat=new SimpleDateFormat("yyyyMMddHHmmss",Locale.getDefault());

    public static String getCurrentTime(){
        return format.format(new Date());
    }

}
