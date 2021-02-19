# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile
-optimizationpasses 5
-dontusemixedcaseclassnames
-dontskipnonpubliclibraryclasses
-dontpreverify
-verbose
-optimizations !code/simplification/arithmetic,!field/*,!class/merging/*

#忽略警告
-ignorewarnings
#保证是独立的jar,没有任何项目引用,如果不写就会认为我们所有的代码是无用的,从而把所有的代码压缩掉,导出一个空的jar
-dontshrink
#保护泛型
-keepattributes Signature
-keep class cn.wch.blelib.host.core.ConnRuler {*;}
-keep class cn.wch.blelib.host.scan.ScanRuler {*;}
-keep class cn.wch.blelib.utils.LogUtil {*;}
-keep class cn.wch.blelib.utils.Location {*;}
-keep class cn.wch.blelib.utils.BLEUtil {*;}
-keep class cn.wch.blelib.utils.FormatUtil {*;}
-keep class cn.wch.blelib.utils.AppUtil {*;}
-keep class cn.wch.blelib.utils.FileUtil {*;}
-keep class cn.wch.blelib.host.core.callback.MtuCallback {*;}
-keep interface cn.wch.blelib.host.scan.ScanObserver{*;}
#CH9140
-keep public class cn.wch.blelib.ch9140.CH9140BluetoothManager {public*;}
-keep class cn.wch.blelib.exception.BLELibException {*;}

-keep interface cn.wch.blelib.ch9140.callback.ConnectStatus{*;}
-keep interface cn.wch.blelib.ch9140.callback.ModemStatus{*;}
-keep interface cn.wch.blelib.ch9140.callback.ReadNotify{*;}
-keep interface cn.wch.blelib.ch9140.callback.RSSIStatus{*;}