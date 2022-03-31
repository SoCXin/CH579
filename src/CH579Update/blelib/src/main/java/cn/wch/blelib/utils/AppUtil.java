package cn.wch.blelib.utils;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ProviderInfo;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import android.text.TextUtils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import androidx.annotation.NonNull;
import androidx.core.content.FileProvider;

import static android.content.Intent.FLAG_ACTIVITY_NEW_TASK;



public class AppUtil {
    public final static int REQUEST_UNKNOWN_APP=2020;

    private static String getAuthorities(Context context){
        PackageManager packageManager = context.getPackageManager();
        try {
            PackageInfo packageInfo = packageManager.getPackageInfo(context.getPackageName(), PackageManager.GET_PROVIDERS);
            ProviderInfo[] providers = packageInfo.providers;
            if(providers!=null && providers.length!=0 && providers[0].authority!=null){
                return providers[0].authority;
            }else {
                return null;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return null;

    }
    /**
     * 发送邮件
     * @param context 上下文
     * @param email 邮箱地址
     * @param content 邮件内容
     */
    public static void sendEmail(@NonNull Context context ,@NonNull String email,@NonNull String content){
        Intent intent = new Intent(Intent.ACTION_SEND);
        String[] tos = { email };
        //String[] ccs = { "gegeff@gmail.com" };
        //String[] bccs = {"fdafda@gmail.com"};
        intent.putExtra(Intent.EXTRA_EMAIL, tos);
        //intent.putExtra(Intent.EXTRA_CC, ccs);
        //intent.putExtra(Intent.EXTRA_BCC, bccs);
        intent.putExtra(Intent.EXTRA_TEXT, content);
        intent.setType("text/plain");
        intent.setType("message/rfc882");
        Intent.createChooser(intent, "Choose Email Client");
        context.startActivity(intent);
    }

    /**
     * 分享当前APK
     * @param context 上下文
     */
    public static void shareApk(Context context) {

        String authorities = getAuthorities(context);
        if(authorities==null){
            return;
        }
        ApplicationInfo applicationInfo=context.getApplicationInfo();
        File apkFile = new File(applicationInfo.sourceDir);
        Uri uri2;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            uri2= FileProvider.getUriForFile(context,authorities,apkFile);
        }else {
            uri2 = Uri.fromFile(apkFile);
        }
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_SEND);
        intent.setType("application/vnd.android.package-archive");
        intent.putExtra(Intent.EXTRA_STREAM, uri2);
        context.startActivity(intent);
    }

    /**
     * 分享文件
     * @param activity 上下文
     */
    public static void shareTxt(Activity activity,@NonNull String path) {

        File file=new File(path);
        if(file==null || !file.exists() || !file.isFile()){
            return;
        }
        String authorities = getAuthorities(activity);
        if(authorities==null){
            return;
        }
        Uri uri2;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            uri2= FileProvider.getUriForFile(activity,authorities,file);
        }else {
            uri2 = Uri.fromFile(file);
        }
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_STREAM, uri2);
        intent.addFlags(FLAG_ACTIVITY_NEW_TASK);
        //context.startActivity(intent);
        activity.startActivity(Intent.createChooser(intent,"分享文件"));
    }

    /**
     * 获取当前APP版本Code
     * @param context 上下文
     * @return 版本Code
     */
    public static int getVersionCode(Context context) {
        PackageManager manager = context.getPackageManager();
        int code = 0;
        try {
            PackageInfo info = manager.getPackageInfo(context.getPackageName(), 0);
            code = info.versionCode;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return code;
    }

    /**
     * 获取当前APP版本Name
     * @param context 上下文
     * @return 版本Name
     */
    public static String getVersionName(Context context) {
        PackageManager manager = context.getPackageManager();
        String name = null;
        try {
            PackageInfo info = manager.getPackageInfo(context.getPackageName(), 0);
            name = info.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return name;
    }

    public static String readFileFromAsset(Context context,String fileName) {
        StringBuilder sb = new StringBuilder("");
        try {
            AssetManager assetManager=context.getResources().getAssets();
            InputStream inputStream = assetManager.open(fileName);
            byte[] buf = new byte[1024*6];
            int len = inputStream.read(buf);
            while (len > 0) {
                sb.append(new String(buf, 0, len, "utf-8"));
                len = inputStream.read(buf);
            }
            inputStream.close();
            //assetManager.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return sb.toString();
    }

    /**
     * 通过email发送文本文件
     * @param context context 上下文
     * @param filePath 文件路径
     * @param subject 邮件主题名
     */
    public static void sendTxt(@NonNull Context context,@NonNull String filePath,@NonNull String subject){
        String authorities = getAuthorities(context);
        if(authorities==null){
            return;
        }

        Intent intent = new Intent(Intent.ACTION_SEND);
        //String[] tos = { "2713259948@qq.com" };
        //String[] ccs = { "gegeff@gmail.com" };
        //String[] bccs = {"fdafda@gmail.com"};
        //intent.putExtra(Intent.EXTRA_EMAIL, tos);
        //intent.putExtra(Intent.EXTRA_CC, ccs);
        //intent.putExtra(Intent.EXTRA_BCC, bccs);
        intent.putExtra(Intent.EXTRA_TEXT, "");
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.setFlags(FLAG_ACTIVITY_NEW_TASK);
        File file2 = new File(filePath);
        Uri uri2;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            uri2=FileProvider.getUriForFile(context,authorities,file2);
        }else {
            uri2 = Uri.fromFile(file2);
        }
        intent.putExtra(Intent.EXTRA_STREAM, uri2);
        intent.setType("text/plain");
        intent.setType("message/rfc882");
        Intent.createChooser(intent, "Choose Email Client");
        context.startActivity(intent);
    }

    /**
     * 获取当前APP包名
     * @param context 上下文
     * @return APP包名
     */
    public static String getPackageName(Context context){
        PackageManager manager=context.getPackageManager();
        try {
            PackageInfo info=manager.getPackageInfo(context.getPackageName(),0);
            return info.packageName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 跳转到应用商店评分
     *
     * @param context 上下文
     * @param shopPkg 包名
     */
    public static void goAppShop(@NonNull Context context, @NonNull String shopPkg) {
        String myAppPkg=getPackageName(context);
        if (TextUtils.isEmpty(myAppPkg)) {
            return;
        }
        try {
            Uri uri = Uri.parse("market://details?id=" + myAppPkg);
            Intent intent = new Intent(Intent.ACTION_VIEW, uri);
            if (!TextUtils.isEmpty(shopPkg)) {
                intent.setPackage(shopPkg);
            }
            intent.addFlags(FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
        } catch (Exception e) {
            // 如果没有该应用商店，则显示系统弹出的应用商店列表供用户选择
            goAppShop(context,"");
        }
    }

    /**
     * 复制文本到剪切板
     * @param context 上下文
     * @param label 标识
     * @param msg 正文
     * @return 成功与否
     */
    public static boolean copyToClip(Context context,String label,String msg){
        ClipboardManager cm=(ClipboardManager)context.getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData clipData=ClipData.newPlainText(label,msg);
        if(clipData==null ||clipData.getItemCount()==0){
            return false;
        }
        if(cm==null){
            return false;
        }
        cm.setPrimaryClip(clipData);
        return true;
    }

    /**
     * 安装app
     * @param activity 上下文
     * @param request 如果没有安装未知来源App的权限，是否申请
     * @param path 目标App路径
     */
    public static void installApk(@NonNull Activity activity, boolean request,@NonNull String path){
        //检查安装未知应用的权限

        String authorities = getAuthorities(activity);
        if(authorities==null){
            return;
        }

        if(!checkInstallApk(activity,request)){
            LogUtil.d("install: permission deny");
            return;
        }
        LogUtil.d("install: "+path);
        File pFile=new File(path);
        if(!pFile.exists()  || !pFile.isFile()){
            return;
        }
        Intent intent=new Intent(Intent.ACTION_VIEW);
        Uri uri;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            uri=FileProvider.getUriForFile(activity,authorities,pFile);
            intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        }else {
            uri = Uri.fromFile(pFile);
            intent.setFlags(FLAG_ACTIVITY_NEW_TASK);
        }
        intent.setDataAndType(uri, "application/vnd.android.package-archive");
        activity.startActivity(intent);
    }

    /**
     * 检查安装未知来源App的权限
     * @param activity 上下文
     * @param request 如果没有安装未知来源App的权限，是否申请
     * @return
     */
    public static boolean checkInstallApk(@NonNull Activity activity, boolean request){
        if(Build.VERSION.SDK_INT>=Build.VERSION_CODES.O){
            PackageManager packageManager = activity.getPackageManager();
            boolean b = packageManager.canRequestPackageInstalls();
            if(!b){
                if(request) {
                    Uri packageUri = Uri.parse("package:" + getPackageName(activity));
                    Intent intent = new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES, packageUri);
                    activity.startActivityForResult(intent, REQUEST_UNKNOWN_APP);
                }
                return false;
            }
        }
        return true;
    }

    /**
     * 检查安装未知来源App的权限
     * @param context 上下文
     * @param request 如果没有安装未知来源App的权限，是否申请
     * @return
     */
    public static boolean checkInstallApk(@NonNull Context context, boolean request){
        if(Build.VERSION.SDK_INT>=Build.VERSION_CODES.O){
            PackageManager packageManager = context.getPackageManager();
            boolean b = packageManager.canRequestPackageInstalls();
            if(!b){
                if(request) {
                    Uri packageUri = Uri.parse("package:" + getPackageName(context));
                    Intent intent = new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES, packageUri);
                    intent.addFlags(FLAG_ACTIVITY_NEW_TASK);
                    context.startActivity(intent);
                }
                return false;
            }
        }
        return true;
    }

}
