package cn.wch.blelib.host.scan;

import java.util.ArrayList;

public class ScanRuler {
    public String[] Name;
    public String[] MAC;
    public ArrayList<byte[]> scanRecord;
    public int rssiMax=200;
    public int rssiMin=100;
    public boolean union=false;


    private ScanRuler(Builder builder) {
        this.Name = builder.Name;
        this.MAC = builder.MAC;
        this.scanRecord = builder.scanRecord;
        this.rssiMax = builder.rssiMax;
        this.rssiMin = builder.rssiMin;
        this.union=builder.union;
    }

    public static class Builder{
        private String[] Name;
        private String[] MAC;
        private ArrayList<byte[]> scanRecord;
        private int rssiMax=200;
        private int rssiMin=100;
        private boolean union=false;


        public Builder Name(String... name) {
            Name = name;
            return this;
        }

        public Builder MAC(String... MAC) {
            this.MAC = MAC;
            return this;
        }

        public Builder ScanRecord(byte[]... scanRecord) {
            this.scanRecord=new ArrayList<>();
            if(scanRecord!=null){
                for(int i=0;i<scanRecord.length;i++){
                    this.scanRecord.add(scanRecord[i]);
                }
            }
            return this;
        }

        public Builder Rssi(int rssiMin,int rssiMax) {
            this.rssiMin = rssiMin;
            this.rssiMax = rssiMax;
            return this;
        }

        public Builder union(boolean union) {
            this.union = union;
            return this;
        }

        public ScanRuler build(){
            return new ScanRuler(this);
        }
    }

}
