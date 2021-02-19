package cn.wch.otaupdate;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;

import com.touchmcu.ui.DialogUtil;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import cn.wch.blelib.ch579.callback.ConnectStatus;
import cn.wch.blelib.ch579.ota.CH579OTAManager;
import cn.wch.blelib.ch579.ota.callback.IProgress;
import cn.wch.blelib.ch579.ota.entry.CurrentImageInfo;
import cn.wch.blelib.ch579.ota.entry.ImageType;
import cn.wch.blelib.exception.BLELibException;
import cn.wch.blelib.utils.LogUtil;
import cn.wch.otaupdate.other.Constant;
import cn.wch.otaupdate.other.ImageFile;
import cn.wch.otaupdate.other.TimeUtil;
import cn.wch.otaupdate.ui.FileListDialog;
import io.reactivex.Observable;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import io.reactivex.Observer;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

import android.os.Handler;
import android.os.Looper;
import android.text.method.ScrollingMovementMethod;
import android.view.Menu;
import android.view.View;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Locale;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * 该模块针对CH579
 */
public class MainActivity extends AppCompatActivity {

    private String address;

    private TextView tv_target;
    private TextView tv_version;
    private TextView tv_offset;
    private TextView tv_new;
    private Button getInfo;
    private Button selectA;
    private Button selectB;
    private TextView monitor;
    private ProgressBar progressBar;
    private TextView tvLog;
    private Button start;

    private CurrentImageInfo currentImageInfo;
    private File targetFile;

    private Handler handler=new Handler(Looper.getMainLooper());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle("OTA升级");
        ;
        setSupportActionBar(toolbar);
        ActionBar supportActionBar = getSupportActionBar();
        if (supportActionBar != null) {
            supportActionBar.setHomeButtonEnabled(true);
            supportActionBar.setDisplayHomeAsUpEnabled(true);
        }
        initWidget();
        init();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main,menu);
        menu.findItem(R.id.connect).setVisible(!CH579OTAManager.getInstance().isConnected(address));
        menu.findItem(R.id.disconnect).setVisible(CH579OTAManager.getInstance().isConnected(address));
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        int itemId = item.getItemId();
        if(itemId==android.R.id.home){
            onBackPressed();
        }else if(itemId==R.id.connect){
            connect();
        }else if(itemId==R.id.disconnect){
            try {
                CH579OTAManager.getInstance().cancel();
                CH579OTAManager.getInstance().disconnect(address,false);
            } catch (BLELibException e) {
                e.printStackTrace();
            }
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed() {
        if(CH579OTAManager.getInstance().isConnected(address)){
            DialogUtil.getInstance().showSimpleDialog(MainActivity.this, "蓝牙已经连接，确定断开？", "断开", "算了", new DialogUtil.IResult() {
                @Override
                public void onContinue() {
                    CH579OTAManager.getInstance().cancel();
                    try {
                        CH579OTAManager.getInstance().disconnect(address,false);
                    } catch (BLELibException e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onCancel() {

                }
            });
            return;
        }
        stopMonitor();
        super.onBackPressed();
    }


    private void initWidget() {
        tv_target = findViewById(R.id.tw_target);
        tv_version = findViewById(R.id.tw_version);
        tv_offset = findViewById(R.id.tw_offset);
        tv_new = findViewById(R.id.tw_file);
        getInfo = findViewById(R.id.btn_getinfo);
        selectA = findViewById(R.id.btn_load_a);
        selectB = findViewById(R.id.btn_load_b);
        progressBar = findViewById(R.id.pb_progress);
        tvLog = findViewById(R.id.tw_log);
        start = findViewById(R.id.btn_start);
        monitor = findViewById(R.id.monitor);

        tvLog.setMovementMethod(ScrollingMovementMethod.getInstance());

        getInfo.setEnabled(false);
        selectA.setEnabled(false);
        selectB.setEnabled(false);
        start.setEnabled(false);

        getInfo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getTargetImageInfo();
            }
        });
        selectA.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                loadFile(ImageFile.A);
            }
        });
        selectB.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                loadFile(ImageFile.B);
            }
        });
        start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(start.getText().toString().equalsIgnoreCase(Constant.START)){
                    start();
                }else if(start.getText().toString().equalsIgnoreCase(Constant.CANCEL)){
                    cancel();
                }
            }
        });
    }

    private void init() {
        if (getIntent() != null) {
            address = getIntent().getStringExtra(Constant.ADDRESS);
        }

        if (address == null) {
            LogUtil.d("mac address is null");
            return;
        }
        connect();
    }

    private void connect(){
        //开始连接
        try {
            CH579OTAManager.getInstance().connect(address, 15000, new ConnectStatus() {
                @Override
                public void OnError(Throwable t) {
                    showToast(t.getMessage());
                }

                @Override
                public void OnConnecting() {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            DialogUtil.getInstance().showLoadingDialog(MainActivity.this, "正在连接");
                        }
                    });

                }

                @Override
                public void OnConnectSuccess(String mac) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            getInfo.setEnabled(true);
                            progressBar.setProgress(0);
                            DialogUtil.getInstance().hideLoadingDialog();
                            invalidateOptionsMenu();
                        }
                    });
                    clearLog();
                    updateLog(mac+" is connected"+"\r\n");
                    startMonitor();
                }

                @Override
                public void onInvalidDevice(String mac) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            DialogUtil.getInstance().hideLoadingDialog();
                            showToast("该设备不是目标设备CH573");
                            try {
                                CH579OTAManager.getInstance().disconnect(address, false);
                            } catch (BLELibException e) {
                                e.printStackTrace();
                            }
                        }
                    });

                }

                @Override
                public void OnConnectTimeout(String mac) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            DialogUtil.getInstance().hideLoadingDialog();
                            invalidateOptionsMenu();
                        }
                    });
                    showToast("连接超时");
                }

                @Override
                public void OnDisconnect(String mac, int status) {
                    showToast("蓝牙已断开连接");
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            getInfo.setEnabled(false);
                            selectA.setEnabled(false);
                            selectB.setEnabled(false);
                            start.setEnabled(false);
                            invalidateOptionsMenu();
                        }
                    });
                    updateLog(mac+" is disconnected"+"\r\n");
                    stopMonitor();
                }
            });
        } catch (BLELibException e) {
            e.printStackTrace();
        }
    }
    private void getTargetImageInfo() {
        Observable.create(new ObservableOnSubscribe<String>() {
            @Override
            public void subscribe(ObservableEmitter<String> emitter) throws Exception {
                currentImageInfo = CH579OTAManager.getInstance().getCurrentImageInfo();
                if (currentImageInfo == null) {
                    emitter.onError(new Throwable("获取Image信息失败"));
                    return;
                }
                emitter.onComplete();
            }
        }).subscribeOn(Schedulers.newThread())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Observer<String>() {
                    @Override
                    public void onSubscribe(Disposable d) {
                        DialogUtil.getInstance().showLoadingDialog(MainActivity.this, "获取Image信息...");
                    }

                    @Override
                    public void onNext(String s) {

                    }

                    @Override
                    public void onError(Throwable e) {
                        DialogUtil.getInstance().hideLoadingDialog();
                        LogUtil.d(e.getMessage());
                        showToast(e.getMessage());
                        selectA.setEnabled(false);
                        selectB.setEnabled(false);
                        start.setEnabled(false);
                        tv_target.setText("null");
                        tv_version.setText("null");
                        tv_offset.setText("null");
                        tv_new.setText("null");
                    }

                    @Override
                    public void onComplete() {
                        DialogUtil.getInstance().hideLoadingDialog();
                        tv_target.setText(currentImageInfo.getType().toString());
                        tv_version.setText(currentImageInfo.getVersion());
                        tv_offset.setText(String.format(Locale.US, "%d", currentImageInfo.getOffset()));
                        start.setEnabled(true);
                        selectA.setEnabled(currentImageInfo.getType()== ImageType.B);
                        selectB.setEnabled(currentImageInfo.getType()== ImageType.A);
                    }
                });
    }

    private void loadFile(ImageFile imageFile) {

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 111);
            return;
        }
        if (!CH579OTAManager.getInstance().createFolder()) {
            showToast("创建文件夹失败");
            return;
        }
        File otaDir = getExternalFilesDir(cn.wch.blelib.ch579.constant.Constant.OTA_FOLDER);
        File image = new File(otaDir, imageFile == ImageFile.A ? cn.wch.blelib.ch579.constant.Constant.OTA_FOLDER_IMAGE_A : cn.wch.blelib.ch579.constant.Constant.OTA_FOLDER_IMAGE_B);
        if (image == null || !image.exists()) {
            showToast("image文件夹不存在");
            return;
        }
        final File[] files = image.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String name) {

                return name.endsWith(".bin") || name.endsWith(".BIN") || name.endsWith(".hex") || name.endsWith(".HEX");
            }
        });
        if (files == null || files.length == 0) {
            showToast("没有找到升级文件");
            return;
        }
        FileListDialog dialog = FileListDialog.newInstance(new ArrayList<File>(Arrays.asList(files)));
        dialog.setCancelable(false);
        dialog.show(getSupportFragmentManager(), FileListDialog.class.getSimpleName());
        dialog.setOnChooseListener(new FileListDialog.OnChooseFileListener() {
            @Override
            public void onChoose(File file) {
                targetFile = file;
                tv_new.setText(String.format(Locale.US, "size: %d", file.length()));
            }
        });
    }

    private void start() {
        Observable.create(new ObservableOnSubscribe<String>() {
            @Override
            public void subscribe(final ObservableEmitter<String> emitter) throws Exception {
                if (currentImageInfo == null) {
                    emitter.onError(new Throwable("image 信息为空"));
                    return;
                }
                if (targetFile == null) {
                    emitter.onError(new Throwable("未选择升级固件"));
                    return;
                }
                if (ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(MainActivity.this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                    // TODO: Consider calling
                    //    ActivityCompat#requestPermissions
                    // here to request the missing permissions, and then overriding
                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                    //                                          int[] grantResults)
                    // to handle the case where the user grants the permission. See the documentation
                    // for ActivityCompat#requestPermissions for more details.
                    emitter.onError(new Throwable("没有读写文件的权限"));
                    return;
                }
                CH579OTAManager.getInstance().start(targetFile, currentImageInfo, new IProgress() {
                    @Override
                    public void onEraseStart() {
                        emitter.onNext("erase start!"+"\r\n");
                    }

                    @Override
                    public void onEraseFinish() {
                        emitter.onNext("erase finish!"+"\r\n");
                    }

                    @Override
                    public void onProgramStart() {
                        resetCount();
                        emitter.onNext("program start!"+"\r\n");
                    }

                    @Override
                    public void onProgramProgress(int current, int total) {
                        updateCount(current);
                        update(current, total);
                    }

                    @Override
                    public void onProgramFinish() {
                        emitter.onNext("program finish!"+"\r\n");
                    }

                    @Override
                    public void onVerifyStart() {
                        resetCount();
                        emitter.onNext("verify start!"+"\r\n");
                    }

                    @Override
                    public void onVerifyProgress(int current, int total) {
                        updateCount(current);
                        update(current, total);
                    }

                    @Override
                    public void onVerifyFinish() {
                        emitter.onNext("verify finish!"+"\r\n");
                    }

                    @Override
                    public void onEnd() {
                        emitter.onNext("end!"+"\r\n");
                        emitter.onComplete();
                    }

                    @Override
                    public void onCancel() {
                        emitter.onNext("cancel!"+"\r\n");
                        emitter.onError(new Throwable("取消升级"));
                    }

                    @Override
                    public void onError(String message) {

                        //emitter.onNext(message+"\r\n");
                        emitter.onError(new Throwable(message));
                    }
                });

            }
        }).subscribeOn(Schedulers.newThread())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Observer<String>() {
                    @Override
                    public void onSubscribe(Disposable d) {
                        //开始升级
                        getInfo.setEnabled(false);
                        selectA.setEnabled(false);
                        selectB.setEnabled(false);
                        start.setEnabled(true);
                        start.setText(Constant.CANCEL);
                        progressBar.setProgress(0);
                    }

                    @Override
                    public void onNext(String s) {
                        updateLog(s);
                    }

                    @Override
                    public void onError(Throwable e) {
                        resetCount();
                        getInfo.setEnabled(true);
                        selectA.setEnabled(false);
                        selectB.setEnabled(false);
                        start.setEnabled(false);
                        start.setText(Constant.START);
                        tv_target.setText("null");
                        tv_version.setText("null");
                        tv_offset.setText("null");
                        tv_new.setText("null");
                        currentImageInfo=null;
                        LogUtil.d(e.getMessage());
                        updateLog(e.getMessage()+"\r\n");
                        showToast(e.getMessage());
                    }

                    @Override
                    public void onComplete() {
                        resetCount();
                        getInfo.setEnabled(true);
                        selectA.setEnabled(false);
                        selectB.setEnabled(false);
                        start.setEnabled(false);
                        start.setText(Constant.START);
                        updateLog("update success!"+"\r\n");
                        LogUtil.d("Complete!");
                    }
                });
    }

    private void cancel(){
        CH579OTAManager.getInstance().cancel();
    }

    private void update(final int current, final int total){
        handler.post(new Runnable() {
            @Override
            public void run() {
                progressBar.setProgress(current*100/total);
            }
        });
    }

    private void updateLog(final String message){
        LogUtil.d("update: "+message);
        handler.post(new Runnable() {
            @Override
            public void run() {
                tvLog.append(TimeUtil.getCurrentTime()+"  "+message);
                int offset = tvLog.getLineCount() * tvLog.getLineHeight();
                //int maxHeight = usbReadValue.getMaxHeight();
                int height = tvLog.getHeight();
                //USBLog.d("offset: "+offset+"  maxHeight: "+maxHeight+" height: "+height);
                if (offset > height) {
                    //USBLog.d("scroll: "+(offset - usbReadValue.getHeight() + usbReadValue.getLineHeight()));
                    tvLog.scrollTo(0, offset - tvLog.getHeight() + tvLog.getLineHeight());
                }
            }
        });
    }

    private void clearLog(){
        handler.post(new Runnable() {
            @Override
            public void run() {
                tvLog.setText("");
                tvLog.scrollTo(0, 0);
            }
        });
    }

    private void showToast(final String message){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this,message,Toast.LENGTH_SHORT).show();
            }
        });
    }

    /////////////////////////////////////speed/////////////////////////////////////////////
    int oldTemp=0;
    int newTemp=0;
    private ScheduledExecutorService scheduledExecutorService;
    private Runnable speedRunnable=new Runnable() {
        @Override
        public void run() {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    int count=newTemp-oldTemp;
                    oldTemp=newTemp;
                    if(count>=0){
                        monitor.setText(String.format(Locale.US,"速度：%d字节/秒",count));
                    }
                }
            });
        }
    };
    void startMonitor(){
        if(speedRunnable==null){
            LogUtil.d("speed monitor is null");
            return;
        }
        stopMonitor();
        LogUtil.d("开始定时器");
        resetCount();
        scheduledExecutorService= Executors.newScheduledThreadPool(2);
        scheduledExecutorService.scheduleWithFixedDelay(speedRunnable,100,1000, TimeUnit.MILLISECONDS);
    }

    void stopMonitor(){

        if(scheduledExecutorService!=null){
            LogUtil.d("取消定时器");
            scheduledExecutorService.shutdown();
            scheduledExecutorService=null;
        }
        resetCount();
    }

    void resetCount(){
        oldTemp=0;
        newTemp=0;
    }

    void updateCount(int newCount){
        newTemp=newCount;
    }
}
