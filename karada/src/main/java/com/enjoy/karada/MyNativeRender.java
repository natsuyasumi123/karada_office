

package com.enjoy.karada;

public class MyNativeRender {
    public static final int TYPE_BASE  =  200;

    public static final int SAMPLE_TYPE_KEY_SHRINK_KOSHI      = TYPE_BASE + 0;
//    public static final int SAMPLE_TYPE_KEY_BIG_BREAST      = TYPE_BASE + 1;
//    public static final int SAMPLE_TYPE_KEY_BIG_EYES            = TYPE_BASE + 2;
//    public static final int SAMPLE_TYPE_KEY_FACE_SLENDER        = TYPE_BASE + 3;
//
//
//    public static final int SAMPLE_TYPE_SET_TOUCH_LOC           = TYPE_BASE + 999;
//    public static final int SAMPLE_TYPE_SET_GRAVITY_XY          = TYPE_BASE + 1000;


    static {
        System.loadLibrary("native-render");
    }

    public native void native_Init();

    public native void native_UnInit();

    public native void native_SetParamsInt(int paramType, int value0, int value1);

    public native void native_SetParamsFloat(int paramType, float value0, float value1);

    public native void native_UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    public native void native_SetImageData(int format, int width, int height, byte[] bytes , float[][] landData, float[][] faceData);

    public native void native_SetImageDataWithIndex(int index, int format, int width, int height, byte[] bytes);


    public native void native_OnSurfaceCreated();

    public native void native_OnSurfaceChanged(int width, int height);

    public native void native_OnDrawFrame();

    public native void native_SetMarkData(float[][] landData , float[][] faceData);
    public native void native_SetOutlineData( byte[] bytes ,int format, int width, int height);
    public native void native_SetDegree(float degree);
}
