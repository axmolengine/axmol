/****************************************************************************
 Copyright (c) 2010-2011 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
package org.axmol.lib;

import android.opengl.GLSurfaceView;
import android.view.Display;

import java.util.concurrent.atomic.AtomicLong;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
public class AxmolRenderer implements GLSurfaceView.Renderer {
    // ===========================================================
    // Constants
    // ===========================================================

    private final static long NANOSECONDSPERSECOND = 1000000000L;
    private final static long NANOSECONDSPERMILLISECOND = 1000000L;

    // Reference interval between render requests.
    private static final AtomicLong sAnimationInterval = new AtomicLong(AxmolRenderer.NANOSECONDSPERSECOND / 60);

    // ===========================================================
    // Fields
    // ===========================================================

    private long mNextFrameStartTime = 0; // Note: read and written by VsyncNotifierThread's thread
    private long mVsyncInterval;
    private final AtomicLong mCurrentFrameStartTime = new AtomicLong();

    private int mScreenWidth;
    private int mScreenHeight;
    private boolean mNativeInitCompleted = false;
    private boolean mIsPaused = false;

    // ===========================================================
    // Constructors
    // ===========================================================

    public AxmolRenderer(Display display)
    {
        mVsyncInterval = (long)((double)NANOSECONDSPERSECOND / (double)display.getRefreshRate());
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    public static void setAnimationInterval(float interval) {
        sAnimationInterval.set((long)((double)interval * (double)AxmolRenderer.NANOSECONDSPERSECOND));
    }

    public void setScreenWidthAndHeight(final int surfaceWidth, final int surfaceHeight) {
        this.mScreenWidth = surfaceWidth;
        this.mScreenHeight = surfaceHeight;
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    @Override
    public void onSurfaceCreated(final GL10 GL10, final EGLConfig EGLConfig) {
        AxmolRenderer.nativeInit(this.mScreenWidth, this.mScreenHeight);
        AxmolRenderer.nativeInitEglPresentationTime();

        if (mNativeInitCompleted) {
            // This must be from an OpenGL context loss
            nativeOnContextLost();
        } else {
            mNativeInitCompleted = true;
        }
    }

    @Override
    public void onSurfaceChanged(final GL10 GL10, final int width, final int height) {
        AxmolRenderer.nativeOnSurfaceChanged(width, height);
    }

    // Determines if frame rendering should be requested.
    // Note: this is called in VsyncNotifierThread's thread via AxmolGLSurfaceView.
    public boolean onVsync(long frameTimeNanos) {
        if (frameTimeNanos >= mNextFrameStartTime) {
            long threshold = NANOSECONDSPERMILLISECOND;
            mNextFrameStartTime = frameTimeNanos + sAnimationInterval.get() - threshold;
            mCurrentFrameStartTime.set(frameTimeNanos);
            return true;
        }

        return false;
    }

    @Override
    public void onDrawFrame(final GL10 gl) {
        long frameStartTime = mCurrentFrameStartTime.get();
        long framePresentationTime = frameStartTime + 2 * mVsyncInterval;
        AxmolRenderer.nativeRender(framePresentationTime);
    }

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeTouchesBegin(final int id, final float x, final float y);
    private static native void nativeTouchesEnd(final int id, final float x, final float y);
    private static native void nativeTouchesMove(final int[] ids, final float[] xs, final float[] ys);
    private static native void nativeTouchesCancel(final int[] ids, final float[] xs, final float[] ys);
    private static native boolean nativeKeyEvent(final int keyCode,boolean isPressed);
    private static native void nativeInitEglPresentationTime();
    private static native void nativeRender(final long framePresentationTimeNanos);
    private static native void nativeInit(final int width, final int height);
    private static native void nativeOnContextLost();
    private static native void nativeOnSurfaceChanged(final int width, final int height);
    private static native void nativeOnPause();
    private static native void nativeOnResume();

    public void handleActionDown(final int id, final float x, final float y) {
        AxmolRenderer.nativeTouchesBegin(id, x, y);
    }

    public void handleActionUp(final int id, final float x, final float y) {
        AxmolRenderer.nativeTouchesEnd(id, x, y);
    }

    public void handleActionCancel(final int[] ids, final float[] xs, final float[] ys) {
        AxmolRenderer.nativeTouchesCancel(ids, xs, ys);
    }

    public void handleActionMove(final int[] ids, final float[] xs, final float[] ys) {
        AxmolRenderer.nativeTouchesMove(ids, xs, ys);
    }

    public void handleKeyDown(final int keyCode) {
        AxmolRenderer.nativeKeyEvent(keyCode, true);
    }

    public void handleKeyUp(final int keyCode) {
        AxmolRenderer.nativeKeyEvent(keyCode, false);
    }

    public void handleOnPause() {
        /**
         * onPause may be invoked before onSurfaceCreated,
         * and engine will be initialized correctly after
         * onSurfaceCreated is invoked. Can not invoke any
         * native method before onSurfaceCreated is invoked
         */
        if (!mNativeInitCompleted)
            return;

        AxmolRenderer.nativeOnPause();
        mIsPaused = true;
    }

    public void handleOnResume() {
        if (mIsPaused) {
            AxmolRenderer.nativeOnResume();
            mIsPaused = false;
        }
    }

    private static native void nativeInsertText(final String text);
    private static native void nativeDeleteBackward();
    private static native String nativeGetContentText();

    public void handleInsertText(final String text) {
        AxmolRenderer.nativeInsertText(text);
    }

    public void handleDeleteBackward() {
        AxmolRenderer.nativeDeleteBackward();
    }

    public String getContentText() {
        return AxmolRenderer.nativeGetContentText();
    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================
}
