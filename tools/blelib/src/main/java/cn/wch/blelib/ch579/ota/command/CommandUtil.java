package cn.wch.blelib.ch579.ota.command;

import java.nio.ByteBuffer;

/**
 * 数据部分不足16字节的，用0x00填充
 */
public class CommandUtil {

    public static final byte CMD_ISP_PROGRAM = (byte) 0x80;
    public static final byte CMD_ISP_ERASE = (byte) 0x81;
    public static final byte CMD_ISP_VERIFY = (byte) 0x82;
    public static final byte CMD_ISP_END = (byte) 0x83;
    public static final byte CMD_ISP_INFO = (byte) 0x84;

    private static final int IAP_LEN = 20;

    //573的地址除以16，CH579除以4
    public static byte[] getImageInfoCommand(){
        ByteBuffer byteBuffer=ByteBuffer.allocate(IAP_LEN);
        byteBuffer.put(CMD_ISP_INFO);
        byteBuffer.put((byte)((IAP_LEN-2)));
        return byteBuffer.array();
    }

    public static byte[] getEraseCommand(int addr,int block){
        ByteBuffer byteBuffer=ByteBuffer.allocate(IAP_LEN);
        byteBuffer.put(CMD_ISP_ERASE);
        byteBuffer.put((byte) 0x00);
        byteBuffer.put((byte)(addr/4));
        byteBuffer.put((byte)((addr/4)>>8));
        byteBuffer.put((byte)(block));
        byteBuffer.put((byte)(block >>8));
        return byteBuffer.array();
    }

    public static byte[] getProgrammeCommand(int addr,byte[] data,int offset){
        ByteBuffer byteBuffer=ByteBuffer.allocate(IAP_LEN);
        byteBuffer.put(CMD_ISP_PROGRAM);
        byteBuffer.put((byte) (IAP_LEN-4));
        byteBuffer.put((byte)(addr/4));
        byteBuffer.put((byte)((addr/4)>>8));
        int len=Math.min(IAP_LEN-4,data.length-offset);
        byteBuffer.put(data,offset,len);
        return byteBuffer.array();
    }

    public static int getProgrammeLength(byte[] data,int offset){
        return Math.min(IAP_LEN-4,data.length-offset);
    }

    public static byte[] getVerifyCommand(int addr,byte[] data,int offset){
        ByteBuffer byteBuffer=ByteBuffer.allocate(IAP_LEN);
        byteBuffer.put(CMD_ISP_VERIFY);
        byteBuffer.put((byte) (IAP_LEN-4));
        byteBuffer.put((byte)(addr/4));
        byteBuffer.put((byte)((addr/4)>>8));
        int len=Math.min(IAP_LEN-4,data.length-offset);
        byteBuffer.put(data,offset,len);
        return byteBuffer.array();
    }

    public static int getVerifyLength(byte[] data,int offset){
        return Math.min(IAP_LEN-4,data.length-offset);
    }

    public static byte[] getEndCommand(){
        ByteBuffer byteBuffer=ByteBuffer.allocate(IAP_LEN);
        byteBuffer.put(CMD_ISP_END);
        byteBuffer.put((byte)((IAP_LEN-2)));
        return byteBuffer.array();
    }


}
