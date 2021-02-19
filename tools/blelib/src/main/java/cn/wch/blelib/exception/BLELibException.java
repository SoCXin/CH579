package cn.wch.blelib.exception;

public class BLELibException extends Exception {
    public BLELibException(String message) {
        super("Message: "+message);
    }
}
