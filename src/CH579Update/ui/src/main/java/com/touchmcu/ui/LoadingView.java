package com.touchmcu.ui;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;
import android.view.animation.LinearInterpolator;

import androidx.annotation.Nullable;

public class LoadingView extends View {


    Context mContext;
    float mCentX=0;
    float mCentY=0;
    int[] mCircleColors=new int[]{Color.WHITE,Color.WHITE,Color.WHITE,Color.WHITE,Color.WHITE,Color.WHITE};
    int mBackGroundColor=Color.parseColor("#3D3D41");

    Paint mPaint;
    float r=8;
    float R=50;
    float mAngle=0f;

    private State state;
    private ValueAnimator mValueAnimator;

    private int mWidth=200;
    private int mHeight=250;


    public LoadingView(Context context) {
        this(context,null);
    }

    public LoadingView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs,0);
    }

    public LoadingView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext=context;
        init();
    }

    private void init() {
        mPaint=new Paint(Paint.ANTI_ALIAS_FLAG);

    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);

        switch (widthMode) {
            case MeasureSpec.EXACTLY:
                //相当于match_parent或者一个具体值
                mWidth = width;
                break;
            case MeasureSpec.AT_MOST:
                // 相当于wrap_content ，需要手动测量大小，这里先写死大小
                break;
            case MeasureSpec.UNSPECIFIED:
                //很少会用到
                break;
            default:
                break;
        }
        switch (heightMode) {
            case MeasureSpec.EXACTLY:
                //相当于match_parent或者一个具体值
                mHeight = height;
                break;
            case MeasureSpec.AT_MOST:
                // 相当于wrap_content ，需要手动测量大小，这里先写死大小
                break;
            case MeasureSpec.UNSPECIFIED:
                //很少会用到
                break;
            default:
                break;
        }
        //存储测量好的宽和高
        setMeasuredDimension(mWidth, mHeight);

    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        mCentX=w*1f/2;
        mCentY=h*2f/3;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if(state==null){
            state=new RotateState();
        }
        state.doDraw(canvas);
    }

    abstract class State{
        abstract void doDraw(Canvas canvas);
    }

    class RotateState extends State{

        public RotateState() {
            mValueAnimator=ValueAnimator.ofFloat(0,(float) Math.PI);
            mValueAnimator.setRepeatCount(ValueAnimator.INFINITE);
            mValueAnimator.setDuration(1000);
            mValueAnimator.setInterpolator(new LinearInterpolator());
            mValueAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    mAngle= (float) animation.getAnimatedValue();
                    invalidate();
                }
            });
            mValueAnimator.start();
        }

        @Override
        void doDraw(Canvas canvas) {
            drawBackGround(canvas);
            drawCircle(canvas);
        }

        private void drawCircle(Canvas canvas){
            float rotateAngle=(float) Math.PI *2/mCircleColors.length;
            for (int i = 0; i < mCircleColors.length; i++) {
                //x=r * cos(a) +centX;
                //y=r * sin(a) +centY;
                float angle=i*rotateAngle +mAngle;
                float cx= (float) (Math.cos(angle)*R+mCentX);
                float cy= (float) (Math.sin(angle)*R+mCentY);
                mPaint.setColor(mCircleColors[i]);
                canvas.drawCircle(cx,cy,r,mPaint);
            }
        }

        private void drawBackGround(Canvas canvas){
            canvas.drawColor(mBackGroundColor);
        }
    }

    public static int dp2px(Context context, float dipValue) {
               final float scale = context.getResources().getDisplayMetrics().density;
               return (int) (dipValue * scale + 0.5f);
    }
}
