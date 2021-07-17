package cn.wch.blelib.ch579.constant;

public class Constant {

    //此处未经校验，
    public static final String ServiceUUID="0000ffe0-0000-1000-8000-00805f9b34fb";
    public static final String ReadCharacterUUID="0000ffe1-0000-1000-8000-00805f9b34fb";
    public static final String WriteCharacterUUID="0000ffe2-0000-1000-8000-00805f9b34fb";
    public static final String RWCharacterUUID="0000ffe3-0000-1000-8000-00805f9b34fb";

    //OTA

    public static final String OTA_ServiceUUID="0000fee0-0000-1000-8000-00805f9b34fb";
    public static final String OTA_CharacterUUID="0000fee1-0000-1000-8000-00805f9b34fb";

    //image 升级文件位置，放在getExternalFilesDir()下
    public static final String OTA_FOLDER="CH579OTA";
    public static final String OTA_FOLDER_IMAGE_A="imageA";
    public static final String OTA_FOLDER_IMAGE_B="imageB";

    //

    public static final String INTENT_KEY_ADDRESS="mac";

}
