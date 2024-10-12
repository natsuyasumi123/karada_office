

package com.karada.app;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.hardware.SensorManager;
import androidx.annotation.*;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.net.Uri;
import android.os.Build;
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
import com.google.mediapipe.framework.image.ByteBufferExtractor;
import com.google.mediapipe.tasks.components.containers.Landmark;
import com.google.mediapipe.tasks.vision.facelandmarker.FaceLandmarker;
import com.google.mediapipe.tasks.vision.facelandmarker.FaceLandmarkerResult;
import com.google.mediapipe.tasks.vision.imagesegmenter.ImageSegmenter;
import com.google.mediapipe.tasks.vision.imagesegmenter.ImageSegmenterResult;
import com.karada.app.adapter.MyRecyclerViewAdapter;
import com.google.mediapipe.framework.image.BitmapImageBuilder;
import com.google.mediapipe.framework.image.MPImage;
import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;
import com.google.mediapipe.tasks.core.BaseOptions;
import com.google.mediapipe.tasks.core.Delegate;
import com.google.mediapipe.tasks.vision.core.RunningMode;
import com.google.mediapipe.tasks.vision.poselandmarker.PoseLandmarker;
import com.google.mediapipe.tasks.vision.poselandmarker.PoseLandmarkerResult;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;



import java.io.BufferedWriter;
import java.io.FileWriter;

import static android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY;
import static com.enjoy.karada.MyGLSurfaceView.* ;
import static com.enjoy.karada.MyNativeRender.*;
import static com.karada.app.CommonUtils.saveBitmapToFile;

public class MainActivity extends AppCompatActivity implements ViewTreeObserver.OnGlobalLayoutListener {
    private static final String TAG = "MainActivity";
    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO,
    };

    private static final int PERMISSION_REQUEST_CODE = 1;
    private static final int PICK_MEDIA_REQUEST = 2;
    private static final String[] SAMPLE_TITLES = {
            "腰KOSHI SHRINK" ,
            "胸MUNE BURST ",
            "👀ME ADJUST(unfinished)",
            "☺KAO SLENDER"
    };

    private MyGLSurfaceView mGLSurfaceView;
    private ViewGroup mRootView;
    private int mSampleSelectedIndex = SAMPLE_TYPE_KEY_SHRINK_KOSHI - SAMPLE_TYPE;
    private MyGLRender mGLRender = new MyGLRender();
    private SensorManager mSensorManager;
    private PoseLandmarker landmarker ;
    private FaceLandmarker facemarker ;

    private ImageSegmenter imagesegmenter ;

    //default test file path
    private String defaultPath = "/sdcard/test/" ;
    private String showFilePath = "" ;


//

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mRootView = findViewById(R.id.rootView);
        mRootView.getViewTreeObserver().addOnGlobalLayoutListener(this);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mGLRender.init();
        createLandMarker();
        createFaceMarker() ;
        createImageSegmenter();
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

        CommonUtils.copyAssetsDirToSDCard(this,"image" ,defaultPath   ) ;
        showFilePath = defaultPath + "image/test.PNG" ;
        showImage(  showFilePath) ;
    }

    interface DetectCallback{
        void onFinish(float[][] landMarks , float [][] faceMarks , Bitmap bitmap , int width , int height);
    }


    private void showImage(  String imagePath ){

        detectImage(imagePath, new DetectCallback() {
            @Override
            public void onFinish(float[][] landMarks, float[][] faceMarks , Bitmap mask , int width , int height) {
                InputStream fis;
                try {
                    fis = new FileInputStream(imagePath) ;
                } catch (FileNotFoundException e) {
                    throw new RuntimeException(e);
                }
                Bitmap bitmap = loadRGBAImage(fis);
                runOnUiThread(() -> {
                    if (bitmap != null) {
                        mGLSurfaceView.setAspectRatio(bitmap.getWidth(), bitmap.getHeight());
                        mGLRender.setMarksData(landMarks , faceMarks);//设置人体信息数据
                        mGLRender.setParamsInt(SAMPLE_TYPE, mSampleSelectedIndex + SAMPLE_TYPE, 0);
                        int bytes = bitmap.getByteCount();
                        ByteBuffer buf = ByteBuffer.allocate(bytes);
                        bitmap.copyPixelsToBuffer(buf);
                        byte[] byteArray = buf.array();
                        mGLRender.setImageData(IMAGE_FORMAT_RGBA, bitmap.getWidth(), bitmap.getHeight(), byteArray);//设置图片
                    }
                    if (mask != null) {
                        int bytes = mask.getByteCount();
                        ByteBuffer buf = ByteBuffer.allocate(bytes);
                        mask.copyPixelsToBuffer(buf);
                        byte[] byteArray = buf.array();
                        mGLRender.setOutlineData(byteArray , IMAGE_FORMAT_GRAY, mask.getWidth(), mask.getHeight() );//设置图片
                    }
                });
            }
        });
    }
    private void createLandMarker(){
        float minPoseDetectionConfidence = 0.5f;
        float minPoseTrackingConfidence = 0.5f;
        float minPosePresenceConfidence = 0.5f;
        RunningMode runningMode  = RunningMode.IMAGE;
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
        landmarker = PoseLandmarker.createFromOptions(getApplicationContext() ,options ) ;
    }

    private void createImageSegmenter(){
        BaseOptions.Builder baseOptionsBuilder = BaseOptions.builder();
//        when (currentDelegate) {
//            DELEGATE_CPU -> {
                baseOptionsBuilder.setDelegate(Delegate.CPU);
//            }
//            DELEGATE_GPU -> {
//                baseOptionsBuilder.setDelegate(Delegate.GPU)
//            }
//        }

//        when(currentModel) {
//            MODEL_DEEPLABV3 -> {
                baseOptionsBuilder.setModelAssetPath("mediapipe_models/segmenters/selfie_multiclass.tflite");
//            }
//            MODEL_HAIR_SEGMENTER -> {
//                baseOptionsBuilder.setModelAssetPath(MODEL_HAIR_SEGMENTER_PATH)
//            }
//            MODEL_SELFIE_SEGMENTER -> {
//                baseOptionsBuilder.setModelAssetPath(MODEL_SELFIE_SEGMENTER_PATH)
//            }
//            MODEL_SELFIE_MULTICLASS -> {
//                baseOptionsBuilder.setModelAssetPath(MODEL_SELFIE_MULTICLASS_PATH)
//            }
//        }
        RunningMode runningMode  = RunningMode.IMAGE;
        BaseOptions baseOptions = baseOptionsBuilder.build();
        ImageSegmenter.ImageSegmenterOptions.Builder optionsBuilder = ImageSegmenter.ImageSegmenterOptions.builder()
                .setRunningMode(runningMode)
                .setBaseOptions(baseOptions)
                .setOutputCategoryMask(true)
                .setOutputConfidenceMasks(false) ;

//        if (runningMode == RunningMode.LIVE_STREAM) {
//            optionsBuilder.setResultListener((result, input) -> {
//
//            }).setErrorListener(e -> {
//
//            });
//        }

        ImageSegmenter.ImageSegmenterOptions options = optionsBuilder.build() ;
        imagesegmenter = ImageSegmenter.createFromOptions(getApplicationContext(), options) ;
    }

    private void createFaceMarker(){
        BaseOptions.Builder baseOptionsBuilder = BaseOptions.builder().setModelAssetPath("mediapipe_models/face_landmarker.task") ;
//        BaseOptions baseOptions = baseOptionsBuilder.build() ;

        FaceLandmarker.FaceLandmarkerOptions.Builder optionsBuilder =
                FaceLandmarker.FaceLandmarkerOptions.builder()
                        .setBaseOptions(baseOptionsBuilder.build())
                        .setMinFaceDetectionConfidence(0.5f)
                        .setMinTrackingConfidence(0.5f)
                        .setMinFacePresenceConfidence(0.5f)
                        .setNumFaces(1)
                        .setRunningMode(RunningMode.IMAGE) ;

        FaceLandmarker.FaceLandmarkerOptions options = optionsBuilder.build() ;
        facemarker = FaceLandmarker.createFromOptions(getApplicationContext(), options) ;
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

    private void detectImage(String imagePath , DetectCallback callback){
        Bitmap rawBitmap = BitmapFactory.decodeFile(imagePath) ;
        BitmapImageBuilder imageBuilder = new BitmapImageBuilder(rawBitmap) ;
        MPImage mpImage = imageBuilder.build();
        new Thread(() -> {
            PoseLandmarkerResult poseRet= landmarker.detect(mpImage) ;
            List<List<NormalizedLandmark>> landMarks = poseRet.landmarks() ;
            Optional<List<MPImage>>segMasksOpt =  poseRet.segmentationMasks() ;
            if(segMasksOpt.isPresent()){
                List<MPImage> segMasks = segMasksOpt.get() ;
                for(MPImage segMask : segMasks){
                    int width = segMask.getWidth();
                    int height = segMask.getHeight() ;
                }
            }
            List<List<Landmark>> worldmarks = poseRet.worldLandmarks() ;
            float [][] poseArray = CommonUtils.convertListToArray(landMarks) ;
            FaceLandmarkerResult faceRet = facemarker.detect(mpImage) ;
            List<List<NormalizedLandmark>> faceMarks = faceRet.faceLandmarks();
            float [][] faceArray = CommonUtils.convertListToArray(faceMarks) ;


            ImageSegmenterResult result = imagesegmenter.segment(mpImage) ;
            Optional<MPImage> cateMaskOpt = result.categoryMask();
            Optional<List<MPImage>>confidenceMasksOpt = result.confidenceMasks() ;
            List<Float>scores = result.qualityScores();

            if(confidenceMasksOpt.isPresent()){
                List<MPImage> masks = confidenceMasksOpt.get();
                int index = 0 ;
                for(MPImage maskImage : masks){
                    FloatBuffer results =  ByteBufferExtractor.extract(maskImage).asFloatBuffer() ;
                    Bitmap bitmapResult = createBitmapFromBuffer(results , maskImage.getWidth() , maskImage.getHeight() , true) ;
                    saveBitmapToFile(bitmapResult , "savedImage" +index+ ".jpg") ;
                    index ++ ;
                    Log.w("MainActivity" , "bitmapResult is " + bitmapResult.getByteCount()) ;
                }
            }
            Bitmap maskBitmap = null ;
            int width = 0, height = 0 ;
            if(cateMaskOpt.isPresent()){
                MPImage cateMask = cateMaskOpt.get() ;
                ByteBuffer results =  ByteBufferExtractor.extract(cateMask);
                width = cateMask.getWidth() ;
                height = cateMask.getHeight() ;
                maskBitmap = CommonUtils.createBitmapFromByteBuffer(results , width ,height) ;
//                saveBitmapToFile(bitmapResult , "cateMask.jpg") ;
            }

            callback.onFinish(poseArray , faceArray , maskBitmap,width , height );
        }).start();
    }

    private Bitmap createBitmapFromBuffer(FloatBuffer floatBuffer,
                                          int width,
                                          int height , boolean isFloat ){
        width = width / 3 ;
        height = height /3 ;
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        // 创建一个int数组来存储像素数据
        int[] pixels = new int[width * height];
        for (int i = 0; i < width * height; i++) {
            // 假设FloatBuffer中的每个像素由4个浮点数表示：R, G, B, A
            float r = floatBuffer.get();
            float g = floatBuffer.get();
            float b = floatBuffer.get();
            float a = floatBuffer.get(); // 透明度
            // 将浮点值 (0 - 1) 转换为 0 - 255 的int值
            int red = Math.round(r * 255);
            int green = Math.round(g * 255);
            int blue = Math.round(b * 255);
            int alpha = Math.round(a * 255);
            // 使用Color.argb()函数生成ARGB颜色值
            pixels[i] = (alpha & 0xff) << 24 | (red & 0xff) << 16 | (green & 0xff) << 8 | (blue & 0xff);
        }
        bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
        return bitmap ;
    }


    /**
     * //                Optional<List<MPImage>> segMasks =  result.segmentationMasks() ;
     * //                List<List<Landmark>> worldMarks =  result.worldLandmarks() ;
     *             //Landmark marks[] = (Landmark[]) worldMarks.get(0).toArray();
     * //                float[] retArray = new float[1000];
     * //                int index = 0 ;
     * //                if(landMarks != null ){
     * //                    for(int landIndex = 0 ; landIndex < landMarks.size() ; landIndex ++ ){
     * //                        for(NormalizedLandmark landmark1 : landMarks.get(landIndex)){
     * //                            retArray[index ++ ] = landmark1.x() ;
     * //                            retArray[index ++ ] = landmark1.y() ;
     * //                            retArray[index ++ ] = landmark1.z() ;
     * //                        }
     * //                        saveToTxt(retArray,  "landMark" + landIndex + ".txt");
     * //                    }
     * //                }
     */

    /**
     *
     * @param uri
     * @return nullable
     */
    public String getRealPathFromURI(Uri uri) {
        String path = null;
        if (Build.VERSION.SDK_INT < 11)
            path = RealPathUtils.getRealPathFromURI_BelowAPI11(MainActivity.this, uri);
            // SDK >= 11 && SDK < 19
        else if (Build.VERSION.SDK_INT < 19)
            path = RealPathUtils.getRealPathFromURI_API11to18(MainActivity.this, uri);
            // SDK > 19 (Android 4.4)
        else
            path = RealPathUtils.getRealPathFromURI_API19(MainActivity.this, uri);
        Log.d(TAG, "File Path: " + path);
        // Get the file instance
        return path;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PICK_MEDIA_REQUEST && resultCode == RESULT_OK) {
            if (data != null) {
                Uri uri = data.getData(); // 获取选中的文件的URI
                showFilePath =  getRealPathFromURI( uri ) ;
                showImage( showFilePath);
            }
        }
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
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mGLRender.unInit();
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
        }else if(id == R.id.action_select_image){
            selectImage();
        }
        return true;
    }



    private void selectImage(){

        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*"); // 设置选择的文件类型，*/*表示选择所有类型
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(Intent.createChooser(intent, "选择图片或视频"), PICK_MEDIA_REQUEST);

    }

    private void showGLSampleDialog() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = LayoutInflater.from(this);
        final View rootView = inflater.inflate(R.layout.sample_selected_layout, null);
        final AlertDialog dialog = builder.create();
        Button confirmBtn = rootView.findViewById(R.id.confirm_btn);
        confirmBtn.setOnClickListener(v -> dialog.cancel());
        final RecyclerView resolutionsListView = rootView.findViewById(R.id.resolution_list_view);
        final MyRecyclerViewAdapter myPreviewSizeViewAdapter = new MyRecyclerViewAdapter(this, Arrays.asList(SAMPLE_TITLES));
        myPreviewSizeViewAdapter.setSelectIndex(mSampleSelectedIndex);
        myPreviewSizeViewAdapter.addOnItemClickListener((view, position) -> {
            onFuncItemSelected(position);
            dialog.cancel();
        });
        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        resolutionsListView.setLayoutManager(manager);
        resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
        dialog.show();
        dialog.getWindow().setContentView(rootView);
    }

    private void onFuncItemSelected(int position ){
        mRootView.removeView(mGLSurfaceView);
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.addRule(RelativeLayout.CENTER_IN_PARENT);
        mGLSurfaceView = new MyGLSurfaceView(MainActivity.this, mGLRender);
        mRootView.addView(mGLSurfaceView, lp);
        mGLSurfaceView.setRenderMode(RENDERMODE_CONTINUOUSLY);
        mSampleSelectedIndex = position;
        if (mRootView.getWidth() != mGLSurfaceView.getWidth()
                || mRootView.getHeight() != mGLSurfaceView.getHeight()) {
            mGLSurfaceView.setAspectRatio(mRootView.getWidth(), mRootView.getHeight());
        }
        showImage(showFilePath);
    }

    private Bitmap loadRGBAImage(InputStream is) {
        Bitmap bitmap;
        try {bitmap = BitmapFactory.decodeStream(is);}
        finally {try {is.close();} catch(IOException e) {e.printStackTrace();}}
        return bitmap;
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
