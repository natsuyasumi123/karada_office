package com.karada.app;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class DraggableQuadView extends View {
    private Paint paint;
    private Path path;
    private VertexListener vertextListener ;
    private int sWidth , sHeight ;

    // 定义四个点的坐标
    private float[] points = {
            200, 200,  // 第一个点 (x1, y1)
            600, 200,  // 第二个点 (x2, y2)
            600, 600,  // 第三个点 (x3, y3)
            200, 600   // 第四个点 (x4, y4)
    };

    public static float crossProduct(float x , float y , float x1 , float y1 , float x2 , float y2) {
        return (x2 - x1) * (y - y1) - (y2 - y1) * (x- x1);
    }

    // 判断点是否在四边形内部
    public static boolean isPointInQuadrilateral(
            float x , float y,
            float[]  p ) {
        float cp1 = crossProduct(x, y  , p[0] , p[1] ,p[2] , p[3]);
        float cp2 = crossProduct(x, y  , p[2] , p[3] ,p[4] , p[5]);
        float cp3 = crossProduct(x, y  , p[4] , p[5] ,p[6] , p[7]);
        float cp4 = crossProduct(x, y  , p[6] , p[7] ,p[0] , p[1]);
        // 判断叉积的符号，如果所有的符号相同，则点在四边形内
        return (cp1 > 0 && cp2 > 0 && cp3 > 0 && cp4 > 0) || (cp1 < 0 && cp2 < 0 && cp3 < 0 && cp4 < 0);
    }


    private float[] centerOffset = {0 , 0} ;

    private static final int RADIUS = 20; // 点的半径
    private int draggingPoint = -1;       // 当前拖拽的点索引
    private boolean draw = true ;

    public DraggableQuadView(Context context) {
        super(context);
        init();
    }

    public DraggableQuadView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public DraggableQuadView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public void setInitPoints(float [] points ){
        this .points = points ;
        invalidate();
    }

    public void setVertextListener(VertexListener lkis){
        this.vertextListener = lkis ;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        sWidth = w ;
        sHeight = h ;
    }

    // 初始化画笔
    private void init() {
        paint = new Paint();
        paint.setColor(Color.WHITE);
        paint.setStrokeWidth(5);
        paint.setStyle(Paint.Style.FILL_AND_STROKE);
        paint.setAntiAlias(true);
        path = new Path();

//        setClickable(true);
//        setOnClickListener(view -> {
//            draw = !draw ;
//            invalidate();
//        });
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if(!draw) return ;
        // 绘制四个点
        for (int i = 0; i < points.length; i += 2) {
            canvas.drawCircle(points[i], points[i + 1], RADIUS, paint);
        }

        // 连接四个点，绘制四边形
        paint.setStyle(Paint.Style.STROKE);
        path.reset();
        path.moveTo(points[0], points[1]);  // 第一个点
        path.lineTo(points[2], points[3]);  // 第二个点
        path.lineTo(points[4], points[5]);  // 第三个点
        path.lineTo(points[6], points[7]);  // 第四个点
        path.close(); // 闭合路径
        canvas.drawPath(path, paint);

        // 恢复为填充样式，供绘制圆点
        paint.setStyle(Paint.Style.FILL);
    }

    float lastX = 0 , lastY = 0 ;
    private int moveCount = 0 ;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                // 检查是否点中了某个点
                draggingPoint = getTouchedPoint(x, y);
                lastX = x ;
                lastY = y ;
                break;
            case MotionEvent.ACTION_MOVE:
                // 如果正在拖拽某个点，更新该点坐标
                if (draggingPoint != -1) {
                    if(draggingPoint == 100){//move
                        float gx = (x - lastX)  ;
                        float gy = (y - lastY)  ;
                        for(int index = 0 ; index < 8 ; index ++  ){
                            if(index % 2 == 0)
                                points[index] += gx ;
                            else
                                points[index] += gy ;
                        }
                        lastX = x ;
                        lastY = y ;
                    }else{ // drag vertices
                        points[draggingPoint * 2] = x;
                        points[draggingPoint * 2 + 1] = y;
                    }

                    invalidate(); // 重绘 View

                    if(vertextListener != null){
                        vertextListener.onDragged(
                                points[0] / sWidth , points[1] / sHeight,
                                points[2] / sWidth , points[3] / sHeight,
                                points[4] / sWidth , points[5] / sHeight,
                                points[6] / sWidth , points[7] / sHeight
                        );
                    }
                    moveCount ++ ;
                }
                break;
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP:
                // 拖拽结束
                if(moveCount < 3 && draggingPoint < 0){
                    draw = !draw ;
                    invalidate();
                }
                moveCount = 0 ;
                draggingPoint = -1;
                break;
        }

        return draggingPoint>= 0 || super.onTouchEvent(event) ;
    }

    // 检查触摸位置是否在某个点附近
    private int getTouchedPoint(float x, float y) {
        float distance = 10000 ;
        for (int i = 0; i < points.length; i += 2) {
            float px = points[i];
            float py = points[i + 1];
            float dis = (float) Math.sqrt(Math.pow(x - px, 2) + Math.pow(y - py, 2));

            if (dis <= RADIUS * 2) {
                return i / 2;
            }
            distance = Math.min(distance , dis) ;
        }
        if(distance < RADIUS * 10 || isPointInQuadrilateral(x , y , points))
        {
            return 100 ;
        }

        return -1;
    }

    public interface VertexListener{
        void onDragged(float x1 , float y1 , float x2 , float y2 , float x3 , float y3 , float x4 , float y4) ;
    }



}
