package cn.wch.ch579update.scan.ui;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import cn.wch.ch579update.R;


public class DeviceAdapter extends RecyclerView.Adapter<DeviceAdapter.MyHolderView> {

    private LayoutInflater inflater;
    private List<BluetoothDevice> deviceList;
    private Map<String, Integer> rssiMap;
    private Listener listener;

    public DeviceAdapter(Context context, Listener onClickListener) {
        inflater=LayoutInflater.from(context);
        deviceList=new ArrayList<>();
        rssiMap=new HashMap<>();
        this.listener=onClickListener;
    }

    @NonNull
    @Override
    public DeviceAdapter.MyHolderView onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new MyHolderView(inflater.inflate(R.layout.devicelist_item,parent,false));
    }

    @Override
    public void onBindViewHolder(@NonNull DeviceAdapter.MyHolderView holder, int position) {
        BluetoothDevice device = deviceList.get(position);
        holder.name.setText(device.getName()==null ? "null":device.getName());
        holder.mac.setText(device.getAddress());

        holder.rssi.setText(rssiMap==null ? "Null" : rssiMap.get(device.getAddress())+"db");
        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(listener!=null){
                    listener.onClick(device);
                }
            }
        });
    }

    @Override
    public void onBindViewHolder(@NonNull MyHolderView holder, int position, @NonNull List<Object> payloads) {
        if(payloads.isEmpty()){
            onBindViewHolder(holder,position);
        }else{
            MyHolderView myViewHolder=(MyHolderView)holder;
            if(payloads.get(0)!=null) {
                if(payloads.get(0) instanceof RSSIMSG){
                    RSSIMSG msg= (RSSIMSG) payloads.get(0);
                    myViewHolder.rssi.setText(msg.getRssi()+" db");
                }

            }

        }
    }

    @Override
    public int getItemCount() {
        return deviceList.size();
    }

    public void update(BluetoothDevice device,int rssi){
        if(deviceList==null){
            return;
        }
        for (int i=0;i<deviceList.size();i++) {
            BluetoothDevice bluetoothDevice=deviceList.get(i);
            if(bluetoothDevice.getAddress().equalsIgnoreCase(device.getAddress())){
                //更新rssi值
                rssiMap.put(device.getAddress(),rssi);
                notifyItemChanged(i,new RSSIMSG(bluetoothDevice.getAddress(),rssi));
                return;
            }
        }
        deviceList.add(device);
        rssiMap.put(device.getAddress(),rssi);
        notifyItemInserted(getItemCount());
    }

    public void clear(){
        deviceList.clear();
        rssiMap.clear();
        notifyDataSetChanged();
    }
    public static class MyHolderView extends RecyclerView.ViewHolder{
        TextView name;
        TextView rssi;
        TextView mac;
        public MyHolderView(@NonNull View itemView) {
            super(itemView);
            rssi = itemView.findViewById(R.id.rssi);
            mac = itemView.findViewById(R.id.mac);
            name = itemView.findViewById(R.id.name);

        }
    }

    public static interface Listener{
        void onClick(BluetoothDevice device);
    }


    public class RSSIMSG{
        String address;
        int rssi;

        public RSSIMSG(String address, int rssi) {
            this.address = address;
            this.rssi = rssi;
        }

        public int getRssi() {
            return rssi;
        }

        public void setRssi(int rssi) {
            this.rssi = rssi;
        }

        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }
    }
}
