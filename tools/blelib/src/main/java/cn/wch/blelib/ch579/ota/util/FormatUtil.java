package cn.wch.blelib.ch579.ota.util;

public class FormatUtil {

    public static int bytesToIntLittleEndian(byte a[], int start) {
        int s = 0;
        int s0 = a[start + 3] & 0xff;
        int s1 = a[start + 2] & 0xff;
        int s2 = a[start + 1] & 0xff;
        int s3 = a[start + 0] & 0xff;
        s0 <<= 24;
        s1 <<= 16;
        s2 <<= 8;
        s = s0 | s1 | s2 | s3;
        return s;
    }
}
