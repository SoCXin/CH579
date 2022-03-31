package cn.wch.blelib.utils;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import androidx.annotation.NonNull;

public class FileUtil {
    public static boolean createFile(String filePath, boolean cover){
        if(filePath.contains(File.separator)){
            String dirpath=filePath.substring(0,filePath.lastIndexOf(File.separator));
            if(!createDir(dirpath,false)){
                LogUtil.d("createDir "+false);
                return false;
            }
        }else {
            return false;
        }
        boolean makeFile=false;
        File file=new File(filePath);
        try{
            if(!file.exists()){
                makeFile=file.createNewFile();
            }else if(cover){//覆盖当前文件
                makeFile=file.delete();
                if(!makeFile){
                    return false;
                }
                makeFile=file.createNewFile();
            }else{//不覆盖当前文件
                return true;
            }
        }catch (IOException e){
            e.printStackTrace();
        }
        return makeFile;
    }
    public static boolean createDir(String dirPath,boolean cover){
        LogUtil.d("createDir "+dirPath);
        boolean makeDir=true;
        File dir=new File(dirPath);
        if(!dir.exists()){//不存在文件夹
            LogUtil.d("createDir "+dir.getAbsolutePath());
            makeDir=dir.mkdir();
        }else if(cover){//覆盖当前文件夹
            makeDir=dir.delete();
            if(!makeDir){
                return false;
            }
            makeDir=dir.mkdir();
        }else{//不覆盖

        }
        return makeDir ;
    }

    public static void writeDataToFile(@NonNull String fileName, byte[] data, FileOutputStream os){
        LogUtil.d("writeDataToFile "+fileName);
        if(data==null || data.length==0 || fileName.equals("")){
            return ;
        }
        File file=new File(fileName);
        if(!file.exists()){
            LogUtil.d("file not exist");
            if(!createFile(fileName,false)){
                return ;
            }

        }
        FileOutputStream ios= os;
        if(ios==null){
            try {
                ios = new FileOutputStream(file,true);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
        }
        try {
            ios.write(data);
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void writeDataToFile(@NonNull String fileName, byte[] data) throws Exception{
        LogUtil.d("writeDataToFile "+fileName);
        if(data==null || data.length==0 || fileName.equals("")){
            return ;
        }
        File file=new File(fileName);
        if(!file.exists()){
            LogUtil.d("file not exist");
            if(!createFile(fileName,false)){
                return ;
            }
            file=new File(fileName);
        }
        FileOutputStream ios= null;

        ios = new FileOutputStream(file,true);
        ios.write(data);
        ios.close();
    }

}
