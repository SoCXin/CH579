package cn.wch.blelib.ch579.ota.util;

import android.content.Context;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import cn.wch.blelib.utils.FormatUtil;

public class DebugUtil {
    private static DebugUtil debugUtil;
    private FileWriter writer;

    public static DebugUtil getInstance() {
        if(debugUtil==null){
            synchronized (DebugUtil.class){
                debugUtil=new DebugUtil();
            }
        }
        return debugUtil;
    }

    public void init(Context context) throws IOException {
        File content = context.getExternalFilesDir("Content");
        File file=new File(content,"记录.txt");
        if(!file.exists()){
            file.createNewFile();
        }
        writer=new FileWriter(file);
    }

    public void write(byte[] data){
        if(writer!=null){
            try {
                writer.write(FormatUtil.bytesToHexString(data)+"\r\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    public void write(String data){
        if(writer!=null){
            try {
                writer.write(data+"\r\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    public void close() throws IOException {
        if(writer!=null){
            writer.flush();
            writer.close();
        }
    }

}
