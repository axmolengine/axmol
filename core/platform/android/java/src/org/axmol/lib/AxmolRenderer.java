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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
public class AxmolRenderer implements GLSurfaceView.Renderer {

    private static class RendererThread extends Thread {

        private GLSurfaceView gameSurfaceView;
        private AxmolRenderer renderer;
        private boolean pausing = false;
        private Object drawMonitor = new Object();
        private Object pauseMonitor = new Object();

        public RendererThread(GLSurfaceView gameSurfaceView, AxmolRenderer renderer) {
            this.gameSurfaceView = gameSurfaceView;
            this.renderer = renderer;
        }

        public void onPause(){
            pausing = true;
        }

        public void onResume(){
            if (pausing) {
                pausing = false;
                synchronized (pauseMonitor) {
                    pauseMonitor.notify();
                }
            }
        }

        public void drawFrameDone() {
            synchronized (drawMonitor) {
                drawMonitor.notify();
            }
        }

        @Override
        public void run() {

            while (true) {
                try {
                    synchronized (pauseMonitor) {
                        if (pausing) {
                            pauseMonitor.wait();
                        }
                    }

                    long now = System.nanoTime();
                    synchronized (drawMonitor) {
                        this.gameSurfaceView.requestRender();
                        drawMonitor.wait();
                    }
                    long wait = (AxmolRenderer.sAnimationInterval - (System.nanoTime()-now)) / AxmolRenderer.NANOSECONDSPERMICROSECOND;
                    if (wait > 0) {
                        sleep(wait);
                    }
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }

    // ===========================================================
    // Constants
    // ===========================================================

    private final static long NANOSECONDSPERSECOND = 1000000000L;
    private final static long NANOSECONDSPERMICROSECOND = 1000000L;

    // The final animation interval which is used in 'onDrawFrame'
    private static long sAnimationInterval = (long) (1.0f / 60f * AxmolRenderer.NANOSECONDSPERSECOND);
    private static float FPS_CONTROL_THRESHOLD = 1.0f / 1200.0f * AxmolRenderer.NANOSECONDSPERSECOND;

    // ===========================================================
    // Fields
    // ===========================================================

    private int mScreenWidth;
    private int mScreenHeight;
    private boolean mNativeInitCompleted = false;
    private boolean mIsPaused = false;
    private RendererThread mRendererThread = null;

    // ===========================================================
    // Constructors
    // ===========================================================

    public AxmolRenderer(GLSurfaceView surfaceView){

        super();
        this.mRendererThread = new RendererThread(surfaceView, this);
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    public static void setAnimationInterval(float interval) {
        sAnimationInterval = (long) (interval * AxmolRenderer.NANOSECONDSPERSECOND);
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
        mRendererThread.start();

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

    @Override
    public void onDrawFrame(final GL10 gl) {
        AxmolRenderer.nativeRender();
        mRendererThread.drawFrameDone();
    }

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeTouchesBegin(final int id, final float x, final float y);
    private static native void nativeTouchesEnd(final int id, final float x, final float y);
    private static native void nativeTouchesMove(final int[] ids, final float[] xs, final float[] ys);
    private static native void nativeTouchesCancel(final int[] ids, final float[] xs, final float[] ys);
    private static native boolean nativeKeyEvent(final int keyCode,boolean isPressed);
    private static native void nativeRender();
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

        mRendererThread.onPause();
        AxmolRenderer.nativeOnPause();
        mIsPaused = true;
    }

    public void handleOnResume() {
        if (mIsPaused) {
            mRendererThread.onResume();
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
