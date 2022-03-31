package cn.wch.otaupdate.ui;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;

import java.io.File;
import java.util.ArrayList;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import cn.wch.otaupdate.R;

public class FileListDialog extends DialogFragment {
    private ArrayList<File> list;
    private RecyclerView recyclerView;
    private FileListAdapter adapter;
    private Button cancel;
    private OnChooseFileListener listener;

    public static FileListDialog newInstance(ArrayList<File> list){
        return new FileListDialog(list);
    }

    public FileListDialog(ArrayList<File> list) {
        this.list = list;
    }


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        getDialog().requestWindowFeature(Window.FEATURE_NO_TITLE);
        View view = inflater.inflate(R.layout.dialog_filelist, null);
        init(view);
        return view;
    }

    private void init(View view) {
        recyclerView=view.findViewById(R.id.list);
        cancel=view.findViewById(R.id.cancel);
        adapter=new FileListAdapter(getActivity(), list, new FileListAdapter.OnClickListener() {
            @Override
            public void onClick(File file) {
                if(listener!=null){
                    listener.onChoose(file);
                }
                dismiss();
            }
        });
        recyclerView.setLayoutManager(new LinearLayoutManager(getActivity(),LinearLayoutManager.VERTICAL,false));
        recyclerView.setAdapter(adapter);
        DividerItemDecoration decoration=new DividerItemDecoration(getActivity(),DividerItemDecoration.VERTICAL);
        recyclerView.addItemDecoration(decoration);

        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
            }
        });
    }

    public void setOnChooseListener(OnChooseFileListener listener){
        this.listener=listener;
    }

    public static interface OnChooseFileListener{
        void onChoose(File file);
    }


}
