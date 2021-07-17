package cn.wch.blelib.host.core;


import androidx.annotation.NonNull;

public class ConnRuler {
    public String[] MAC;
    public long connectTimeout;
    public boolean autoDiscoverServices;
    public int readNullRetryCount;
    public long sleepTimeBeforeDiscover;
    public long readTimeout;
    public long writeTimeout;

    private ConnRuler(Builder builder) {
        this.MAC = builder.MAC;
        this.connectTimeout = builder.connectTimeout;
        this.autoDiscoverServices=builder.autoDiscoverServices;
        this.readNullRetryCount=builder.readNullRetryCount+1;
        this.sleepTimeBeforeDiscover=builder.sleepTimeBeforeDiscover;
        this.readTimeout=builder.readTimeout;
        this.writeTimeout=builder.writeTimeout;
    }

    public static class Builder {
        private String[] MAC=new String[1];
        private long connectTimeout=15000;
        private boolean autoDiscoverServices=true;
        private int readNullRetryCount=10;
        public long sleepTimeBeforeDiscover=600;
        public long readTimeout=1000;
        public long writeTimeout=1000;

        public Builder MAC(@NonNull String mac) {
            MAC[0] = mac;
            return this;
        }

        public Builder ConnectTimeout(long connectTimeout) {
            this.connectTimeout = connectTimeout;
            return this;
        }



        public Builder readTimeout(long readTimeout) {
            this.readTimeout = readTimeout;
            return this;
        }

        public Builder writeTimeout(long writeTimeout) {
            this.writeTimeout = writeTimeout;
            return this;
        }

        public ConnRuler build(){
            return new ConnRuler(this);
        }
    }
}
