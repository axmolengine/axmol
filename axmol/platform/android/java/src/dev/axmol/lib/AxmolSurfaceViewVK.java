/****************************************************************************
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
package dev.axmol.lib;

import android.opengl.GLSurfaceView;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.content.Context;
import android.util.Log;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Vulkan implementation of Axmol render surface.
 * Manages its own render thread similar to GLSurfaceView's behavior.
 */
public class AxmolSurfaceViewVK extends SurfaceView implements AxmolRenderHost, SurfaceHolder.Callback2 {
    private static final String TAG = "AxmolSurfaceViewVK";

    private RenderThread mRenderThread;
    private boolean mIsInitialized = false;
    private final Object mSurfaceLock = new Object();
    private AxmolPlayer mPlayer;

    public AxmolSurfaceViewVK(AxmolPlayer player) {
        super(player.getContext());
        mPlayer = player;
        initView();
    }

    private void initView() {
        // Enable touch events
        setFocusable(true);
        setFocusableInTouchMode(true);

        // Set surface holder callback
        getHolder().addCallback(this);

        setKeepScreenOn(true);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "Surface created");

        synchronized (mSurfaceLock) {
            // Start render thread if not already running
            if (mRenderThread == null) {
                mRenderThread = new RenderThread(holder.getSurface());
                mRenderThread.start();
            }
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "Surface changed: " + width + "x" + height);

        if (mIsInitialized) {
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "Surface destroyed");

        synchronized (mSurfaceLock) {
            // Stop render thread
            if (mRenderThread != null) {
                mRenderThread.requestExitAndWait();
                mRenderThread = null;
            }
        }
    }

    @Override
    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        // Request a redraw if needed
        if (mRenderThread != null) {
            mRenderThread.requestRedraw();
        }
    }

    @Override
    public void configureRenderMode(int mode) {
        //super.setRenderMode(mode == AxmolRenderHost.RENDERMODE_CONTINUOUSLY ? GLSurfaceView.RENDERMODE_CONTINUOUSLY : GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void onRenderPause() {
        //super.onPause();
    }

    @Override
    public void onRenderResume() {
        //super.onResume();
    }

    /**
     * Queue a Runnable to be executed on the Vulkan render thread.
     */
    public void queueEvent(Runnable runnable) {
        mRenderThread.queueEvent(runnable);
    }

    /**
     * Dedicated render thread for Vulkan rendering.
     * Mimics GLSurfaceView's render thread behavior.
     */
    private class RenderThread extends Thread {
        private final Surface mSurface;
        private volatile boolean mShouldExit = false;
        private volatile boolean mRequestRedraw = false;
        private final BlockingQueue<Runnable> mEventQueue = new LinkedBlockingQueue<>();

        public RenderThread(Surface surface) {
            super("AxmolVulkanRenderThread");
            mSurface = surface;
        }

        @Override
        public void run() {
            // Initialize Vulkan and Axmol engine
            // initializeAxmol(mSurface, getWidth(), getHeight());

            // Main render loop
            while (!mShouldExit) {
                // Process pending events
                processEvents();

                // Render frame
                if (mIsInitialized) {
                    //  nativeRenderVK();
                }

                // Wait if no redraw requested (similar to RENDERMODE_WHEN_DIRTY)
                if (!mRequestRedraw) {
                    synchronized (this) {
                        try {
                            wait(16); // ~60 FPS default
                        } catch (InterruptedException e) {
                            // Thread interrupted
                        }
                    }
                }
                mRequestRedraw = false;
            }
        }

        private void processEvents() {
            Runnable event;
            while ((event = mEventQueue.poll()) != null) {
                try {
                    event.run();
                } catch (Exception e) {
                    Log.e(TAG, "Exception in render thread event", e);
                }
            }
        }

        public void requestExitAndWait() {
            mShouldExit = true;
            synchronized (this) {
                notify();
            }

            try {
                join(1000); // Wait up to 1 second
            } catch (InterruptedException e) {
                Log.w(TAG, "Interrupted while waiting for render thread to exit");
            }
        }

        public void requestRedraw() {
            mRequestRedraw = true;
            synchronized (this) {
                notify();
            }
        }

        public void queueEvent(Runnable runnable) {
            mEventQueue.offer(runnable);
            // Wake up thread if waiting
            synchronized (this) {
                notify();
            }
        }
    }
}
