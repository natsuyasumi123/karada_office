package com.enjoy.karada;

import static com.enjoy.karada.MyNativeRender.*;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;


public class MyGLSurfaceView extends GLSurfaceView  {
    private static final String TAG = "MyGLSurfaceView";

    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;

    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_GRAY = 0x06;


    private MyGLRender mGLRender;

    private int mRatioWidth = 0;
    private int mRatioHeight = 0;


    public MyGLSurfaceView(Context context, MyGLRender glRender) {
        this(context, glRender, null);
    }

    public MyGLSurfaceView(Context context, MyGLRender glRender, AttributeSet attrs) {
        super(context, attrs);
        this.setEGLContextClientVersion(2);
        mGLRender = glRender;
        /*If no setEGLConfigChooser method is called,
        then by default the view will choose an RGB_888 surface with a depth buffer depth of at least 16 bits.*/
        setEGLConfigChooser(8, 8, 8, 8, 16, 8);
        setRenderer(mGLRender);

    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height);
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
            }
        }
    }

    public void setAspectRatio(int width, int height) {
        Log.d(TAG, "setAspectRatio() called with: width = [" + width + "], height = [" + height + "]");
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Size cannot be negative.");
        }

        mRatioWidth = width;
        mRatioHeight = height;
        requestLayout();
    }

    public MyGLRender getGLRender() {
        return mGLRender;
    }


    public void dealClickEvent(MotionEvent e) {

    }
}
