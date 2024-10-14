package com.enjoy.karada;

import static com.enjoy.karada.MyNativeRender.*;

import android.opengl.GLSurfaceView;
import android.util.Log;

//import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;

import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;



public class MyGLRender implements GLSurfaceView.Renderer {
    private static final String TAG = "MyGLRender";
    private MyNativeRender mNativeRender;
    private int mSampleType;

    public MyGLRender() {
        mNativeRender = new MyNativeRender();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mNativeRender.native_OnSurfaceCreated();
        Log.e(TAG, "onSurfaceCreated() called with: GL_VERSION = [" + gl.glGetString(GL10.GL_VERSION) + "]");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mNativeRender.native_OnSurfaceChanged(width, height);

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        mNativeRender.native_OnDrawFrame();

    }

    public void init() {
        mNativeRender.native_Init();
    }

    public void unInit() {
        mNativeRender.native_UnInit();
    }

    public void setParamsInt(int paramType, int value0, int value1) {
        if (paramType == SAMPLE_TYPE) {
            mSampleType = value0;
        }
        mNativeRender.native_SetParamsInt(paramType, value0, value1);
    }

    public void setImageData(int format, int width, int height, byte[] bytes , float[][] landData ,  float[][] faceMarks) {
        mNativeRender.native_SetImageData(format, width, height, bytes ,landData, faceMarks );
    }

    public void setMarksData(float[][]  poseData , float[][]  faceData){
        mNativeRender.native_SetMarkData(poseData, faceData) ;
    }

    public void setOutlineData(byte[] bytes , int format , int width , int height) {
       // mNativeRender.native_SetOutlineData(bytes , format , width , height) ;
    }

    /**
     * 用户手动设置一些部位的大小和位置
     * @param type 人体部位类型
     * @param poseData 相关的数据  簡単のために、いちこデータ渡ってがいい
     * @param degree
     */
    public void setCustomPosition(int type , float [][] poseData ,  float degree){

    }

    public void setDegree(float degree){
        mNativeRender.native_SetDegree(degree);
    }
}
