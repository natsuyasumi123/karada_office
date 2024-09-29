/**
 *
 * Created by 公众号：字节流动 on 2021/3/12.
 * https://github.com/githubhaohao/NDK_OpenGLES_3_0
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

package com.karada.app;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import androidx.annotation.*;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import android.os.Bundle;
import android.os.Environment;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.enjoy.karada.MyGLRender;
import com.enjoy.karada.MyGLSurfaceView;
import com.karada.app.adapter.MyRecyclerViewAdapter;
import com.karada.app.audio.AudioCollector;
import com.karada.app.egl.EGLActivity;
import com.google.mediapipe.framework.image.BitmapImageBuilder;
import com.google.mediapipe.framework.image.MPImage;
import com.google.mediapipe.framework.image.MPImageProperties;
import com.google.mediapipe.tasks.components.containers.Landmark;
import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;
import com.google.mediapipe.tasks.core.BaseOptions;
import com.google.mediapipe.tasks.core.Delegate;
import com.google.mediapipe.tasks.vision.core.RunningMode;
import com.google.mediapipe.tasks.vision.poselandmarker.PoseLandmarker;
import com.google.mediapipe.tasks.vision.poselandmarker.PoseLandmarkerResult;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;



import java.io.BufferedWriter;
import java.io.FileWriter;

import static android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY;
import static com.enjoy.karada.MyGLSurfaceView.* ;
import static com.enjoy.karada.MyNativeRender.*;

public class MainActivity extends AppCompatActivity implements AudioCollector.Callback, ViewTreeObserver.OnGlobalLayoutListener {
    private static final String TAG = "MainActivity";
    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO,
    };
    private static final int PERMISSION_REQUEST_CODE = 1;
    private static final String[] SAMPLE_TITLES = {
            "腰KOSHI SHRINK" ,
            "胸MUNE BURST ",
            "👀ME ADJUST",
            "☺KAO SLENDER"
    };

    private MyGLSurfaceView mGLSurfaceView;
    private ViewGroup mRootView;
    private int mSampleSelectedIndex = SAMPLE_TYPE_KEY_SHRINK_KOSHI - SAMPLE_TYPE;
    private AudioCollector mAudioCollector;
    private MyGLRender mGLRender = new MyGLRender();
    private SensorManager mSensorManager;
    private PoseLandmarker landmarker ;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mRootView = (ViewGroup) findViewById(R.id.rootView);
        mRootView.getViewTreeObserver().addOnGlobalLayoutListener(this);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mGLRender.init();
        createLandMarker();
    }

    float minPoseDetectionConfidence = 0.5f;
    float minPoseTrackingConfidence = 0.5f;
    float minPosePresenceConfidence = 0.5f;
    RunningMode runningMode  = RunningMode.IMAGE;
    private void createLandMarker(){
        BaseOptions.Builder baseOptionBuilder = BaseOptions.builder() ;
        baseOptionBuilder.setDelegate(Delegate.CPU) ;
        baseOptionBuilder.setModelAssetPath("mediapipe_models/pose_landmarker_full.task");
        BaseOptions baseOptions = baseOptionBuilder.build() ;
        PoseLandmarker.PoseLandmarkerOptions.Builder optionsBuilder =
                PoseLandmarker.PoseLandmarkerOptions.builder()
                        .setBaseOptions(baseOptions)
                        .setMinPoseDetectionConfidence(minPoseDetectionConfidence)
                        .setMinTrackingConfidence(minPoseTrackingConfidence)
                        .setMinPosePresenceConfidence(minPosePresenceConfidence)
                        .setRunningMode(runningMode) ;
        PoseLandmarker.PoseLandmarkerOptions options = optionsBuilder.build() ;
        landmarker = PoseLandmarker.createFromOptions(this ,options ) ;



    }


    public static void saveToTxt(float [ ]retArray , String fileName) {
        // 定义一个float数组
        //float[] retArray = {1.0f, 2.5f, 3.8f, 4.2f, 5.9f};

        // 定义文件路径
        String filePath = "/sdcard/" + fileName;

        // 使用 try-with-resources 来自动关闭流
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath))) {
            // 遍历数组并写入文件
            for (float value : retArray) {
                writer.write(value + ",");  // 将float值转换为字符串
                writer.newLine();  // 每个值写入后换行
            }
            System.out.println("数组已成功保存到文件: " + filePath);
        } catch (IOException e) {
            System.out.println("文件写入出错: " + e.getMessage());
        }
    }

    private void detectImage(){
        Bitmap rawBitmap = BitmapFactory.decodeFile("/sdcard/Materials/e.jpg") ;
//        Matrix matrix = new Matrix() ;
//        matrix.postRotate(0);
//        matrix.postScale(-1f, 1.0f , rawBitmap.getWidth() , rawBitmap.getHeight()) ;
//        Bitmap rotatedBitmap = Bitmap.createBitmap(
//                rawBitmap, 0, 0,
//                rawBitmap.getWidth(), rawBitmap.getHeight(),
//                matrix, true) ;
        BitmapImageBuilder imageBuilder = new BitmapImageBuilder(rawBitmap) ;
        MPImage mpImage = imageBuilder.build();
        long frameTime = SystemClock.uptimeMillis() ;
        new Thread(new Runnable() {
            @Override
            public void run() {
                //landmarker.detectAsync(mpImage , frameTime);
                PoseLandmarkerResult result= landmarker.detect(mpImage) ;
                List<List<NormalizedLandmark>> landMarks = result.landmarks() ;
                Optional<List<MPImage>> segMasks =  result.segmentationMasks() ;
                List<List<Landmark>> worldMarks =  result.worldLandmarks() ;
                //Landmark marks[] = (Landmark[]) worldMarks.get(0).toArray();
                float[] retArray = new float[1000];
                int index = 0 ;
                if(landMarks != null ){
                    for(int landIndex = 0 ; landIndex < landMarks.size() ; landIndex ++ ){
                        for(NormalizedLandmark landmark1 : landMarks.get(landIndex)){
                            retArray[index ++ ] = landmark1.x() ;
                            retArray[index ++ ] = landmark1.y() ;
                            retArray[index ++ ] = landmark1.z() ;
                        }
                        saveToTxt(retArray,  "landMark" + landIndex + ".txt");
                    }

                }


                if(segMasks != null  && segMasks.isPresent()){
                    List<MPImage> segMaskList = segMasks.get();
                    if(segMaskList != null){
                        for(MPImage mpImage : segMaskList){
                            List<MPImageProperties> imageProperties = mpImage.getContainedImageProperties() ;
                            for(MPImageProperties imageProperty : imageProperties){
                                imageProperty.getImageFormat() ;

                            }
                        }
                    }
                }

                if(worldMarks != null){
                    int fileIndex = 0 ;
                    for(List<Landmark> subMarks  : worldMarks){
                        float[] markArray = new float[1000];
                        int i =  0 ;
                        for(Landmark mark : subMarks){
                            if(mark != null){
                                float x=  mark.x() ;
                                float y = mark.y() ;
                                float z = mark.z() ;
                                markArray[i ++ ] = x ;
                                markArray[i ++ ] = y ;
                                markArray[i ++ ] = z ;
                            }
                        }
                        saveToTxt(markArray,  "worldMarks" + fileIndex + ".txt");
                        fileIndex ++ ;
                    }
                }



                Log.e("MainActivity" , "event end " + retArray);
            }
        }).start();

    }

    @Override
    public void onGlobalLayout() {
        mRootView.getViewTreeObserver().removeOnGlobalLayoutListener(this);
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.addRule(RelativeLayout.CENTER_IN_PARENT);
        mGLSurfaceView = new MyGLSurfaceView(this, mGLRender);
        mGLSurfaceView.setRenderMode(RENDERMODE_CONTINUOUSLY);
        mRootView.addView(mGLSurfaceView, lp);
    }



    @Override
    protected void onResume() {
        super.onResume();

        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
        }
        ///sdcard/Android/data/com.karada.app/files/Download
        String fileDir = getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();
        CommonUtils.copyAssetsDirToSDCard(MainActivity.this, "poly", fileDir + "/model");
        CommonUtils.copyAssetsDirToSDCard(MainActivity.this, "fonts", fileDir);
        CommonUtils.copyAssetsDirToSDCard(MainActivity.this, "yuv", fileDir);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(this, "We need the permission: WRITE_EXTERNAL_STORAGE", Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mAudioCollector != null) {
            mAudioCollector.unInit();
            mAudioCollector = null;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mGLRender.unInit();
        /*
        * Once the EGL context gets destroyed all the GL buffers etc will get destroyed with it,
        * so this is unnecessary.
        * */
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_change_sample) {
            showGLSampleDialog();
        }
        return true;
    }

    @Override
    public void onAudioBufferCallback(short[] buffer) {
        Log.e(TAG, "onAudioBufferCallback() called with: buffer[0] = [" + buffer[0] + "]");
        mGLRender.setAudioData(buffer);
        //mGLSurfaceView.requestRender();
    }



    private void showGLSampleDialog() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = LayoutInflater.from(this);
        final View rootView = inflater.inflate(R.layout.sample_selected_layout, null);

        final AlertDialog dialog = builder.create();

        Button confirmBtn = rootView.findViewById(R.id.confirm_btn);
        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.cancel();
            }
        });

        final RecyclerView resolutionsListView = rootView.findViewById(R.id.resolution_list_view);

        final MyRecyclerViewAdapter myPreviewSizeViewAdapter = new MyRecyclerViewAdapter(this, Arrays.asList(SAMPLE_TITLES));
        myPreviewSizeViewAdapter.setSelectIndex(mSampleSelectedIndex);
        myPreviewSizeViewAdapter.addOnItemClickListener(new MyRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                mRootView.removeView(mGLSurfaceView);
                RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
                lp.addRule(RelativeLayout.CENTER_IN_PARENT);
                mGLSurfaceView = new MyGLSurfaceView(MainActivity.this, mGLRender);
                mRootView.addView(mGLSurfaceView, lp);

                int selectIndex = myPreviewSizeViewAdapter.getSelectIndex();
                myPreviewSizeViewAdapter.setSelectIndex(position);
                myPreviewSizeViewAdapter.notifyItemChanged(selectIndex);
                myPreviewSizeViewAdapter.notifyItemChanged(position);
                mSampleSelectedIndex = position;

                if (mRootView.getWidth() != mGLSurfaceView.getWidth()
                        || mRootView.getHeight() != mGLSurfaceView.getHeight()) {
                    mGLSurfaceView.setAspectRatio(mRootView.getWidth(), mRootView.getHeight());
                }

                mGLRender.setParamsInt(SAMPLE_TYPE, position + SAMPLE_TYPE, 0);

                int sampleType = position + SAMPLE_TYPE;

                detectImage() ;
                Bitmap tmp;
                switch (sampleType) {

                    case  SAMPLE_TYPE_KEY_SHRINK_KOSHI:
                        Bitmap bitmap1= loadRGBAImage(R.drawable.test);
                        mGLSurfaceView.setAspectRatio(bitmap1.getWidth(), bitmap1.getHeight());
                        break ;
                    case  SAMPLE_TYPE_KEY_BIG_BREAST:
                        Bitmap bitmap3= loadRGBAImage(R.drawable.test);
                        mGLSurfaceView.setAspectRatio(bitmap3.getWidth(), bitmap3.getHeight());
                        break ;
                    case SAMPLE_TYPE_KEY_BIG_EYES:
                        Bitmap bitmap = loadRGBAImage(R.drawable.yifei);
                        mGLSurfaceView.setAspectRatio(bitmap.getWidth(), bitmap.getHeight());
                        break;
                    case SAMPLE_TYPE_KEY_FACE_SLENDER:
                        Bitmap bitmap2 = loadRGBAImage(R.drawable.test);
                        mGLSurfaceView.setAspectRatio(bitmap2.getWidth(), bitmap2.getHeight());
                        Log.w("SAMPLE_TYPE_KEY_FACE_SLENDER" ,"FACE SLENDER " ) ;
                        break;
                    default:
                        break;
                }

                mGLSurfaceView.requestRender();
                dialog.cancel();
            }
        });

        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        resolutionsListView.setLayoutManager(manager);

        resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
        resolutionsListView.scrollToPosition(mSampleSelectedIndex);

        dialog.show();
        dialog.getWindow().setContentView(rootView);

    }

    private Bitmap loadRGBAImage(int resId) {
        InputStream is = this.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                mGLRender.setImageData(IMAGE_FORMAT_RGBA, bitmap.getWidth(), bitmap.getHeight(), byteArray);
            }
        }
        finally
        {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
        return bitmap;
    }

    private Bitmap loadRGBAImage(int resId, int index) {
        InputStream is = this.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                mGLRender.setImageDataWithIndex(index, IMAGE_FORMAT_RGBA, bitmap.getWidth(), bitmap.getHeight(), byteArray);
            }
        }
        finally
        {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
        return bitmap;
    }

    private void loadNV21Image() {
        InputStream is = null;
        try {
            is = getAssets().open("YUV_Image_840x1074.NV21");
        } catch (IOException e) {
            e.printStackTrace();
        }

        int lenght = 0;
        try {
            lenght = is.available();
            byte[] buffer = new byte[lenght];
            is.read(buffer);
            mGLRender.setImageData(IMAGE_FORMAT_NV21, 840, 1074, buffer);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }

    }

    private void loadGrayImage() {
        InputStream is = null;
        try {
            is = getAssets().open("lye_1280x800.Gray");
        } catch (IOException e) {
            e.printStackTrace();
        }

        int lenght = 0;
        try {
            lenght = is.available();
            byte[] buffer = new byte[lenght];
            is.read(buffer);
            mGLRender.setImageDataWithIndex(0, IMAGE_FORMAT_GARY, 1280, 800, buffer);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }

    }

    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

}
