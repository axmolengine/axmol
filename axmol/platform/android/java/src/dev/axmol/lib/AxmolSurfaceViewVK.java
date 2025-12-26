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

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.util.Log;
import android.view.View;

import java.lang.ref.WeakReference;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Vulkan implementation of Axmol render surface.
 * Manages its own render thread similar to GLSurfaceView's behavior.
 */
public class AxmolSurfaceViewVK extends SurfaceView implements AxmolRenderHost, SurfaceHolder.Callback2 {
    private static final String TAG = "AxmolSurfaceViewVK";

    private static final boolean LOG_ATTACH_DETACH = true;

    // Thread manager for coordinating multiple Vulkan render threads (if needed)
    private static final RenderThreadManager sThreadManager = new RenderThreadManager();

    private RenderThread mRenderThread;
    private boolean mIsInitialized = false;
    private AxmolPlayer mPlayer;

    // Rendering mode control
    private volatile boolean mRenderPaused = false;
    // private volatile boolean mShouldRender = false;
    private boolean mDetached;

    public AxmolSurfaceViewVK(AxmolPlayer player) {
        super(player.getContext());
        init(player);
    }

    private void init(AxmolPlayer player) {
        mPlayer = player;
        // Enable touch events
        setFocusable(true);
        setFocusableInTouchMode(true);
        // Set surface holder callback
        getHolder().addCallback(this);
        // Set Z-order to ensure surface is placed on top of other views
        setZOrderOnTop(true);
        setKeepScreenOn(true);

        // Start render thread if not already running
        mRenderThread = new RenderThread(mThisWeakRef);
        mRenderThread.start();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");

        final Surface surface = holder.getSurface();
        if (surface == null) {
            Log.w(TAG, "surfaceCreated: holder.getSurface() == null");
            return;
        }

        synchronized (sThreadManager) {
            // If render thread is missing or already exited, create and start it.
            if (mRenderThread == null || mRenderThread.hasExited()) {
                mRenderThread = new RenderThread(mThisWeakRef);
                // preserve default render mode or set previously stored mode if you keep one
                mRenderThread.start();
            }

            // Post initialization to the render thread to ensure all native calls
            // (ANativeWindow_fromSurface, vkCreateAndroidSurfaceKHR, etc.) happen on the same thread.
            final RenderThread rt = mRenderThread;
            rt.queueEvent(new Runnable() {
                @Override
                public void run() {
                    try {
                        if (!mIsInitialized) {
                            // Let AxmolPlayer (native) create VkSurfaceKHR using the provided Surface.
                            mPlayer.onSurfaceCreated(surface);
                            mIsInitialized = true;
                            Log.d(TAG, "surfaceCreated: initialized on render thread");
                        } else {
                            Log.d(TAG, "surfaceCreated: already initialized");
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Exception during onSurfaceCreated", e);
                    }
                }
            });
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged: " + width + "x" + height);

        final RenderThread rt;
        synchronized (sThreadManager) {
            rt = mRenderThread;
        }

        // If render thread is not ready, queue a resize event that will run when thread is available.
        if (rt == null || rt.hasExited()) {
            Log.w(TAG, "surfaceChanged: render thread not ready, queueing resize");
            // Use queueEvent to ensure the call runs on the render thread once it's created.
            // If mRenderThread is null now, this will be handled after thread creation in surfaceCreated.
            if (mRenderThread != null) {
                mRenderThread.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeOnSurfaceChanged(width, height);
                    }
                });
            } else {
                // Fallback: start a short-lived runnable on a new thread to avoid dropping the event.
                // Preferably, surfaceCreated will handle initialization and subsequent resize.
                Log.w(TAG, "surfaceChanged: no render thread to accept resize; resize will be handled after create");
            }
            return;
        }

        // Dispatch resize to render thread so swapchain recreation happens on the render thread.
        rt.queueEvent(new Runnable() {
            @Override
            public void run() {
                try {
                    if (!mIsInitialized) {
                        // Defensive: if initialization hasn't happened yet, attempt to initialize first.
                        final Surface s = holder.getSurface();
                        if (s != null) {
                            mPlayer.onSurfaceCreated(s);
                            mIsInitialized = true;
                        }
                    }
                    // Notify native layer about the new surface size (rebuild swapchain there).
                    AxmolPlayer.nativeOnSurfaceChanged(width, height);
                } catch (Exception e) {
                    Log.e(TAG, "Exception during surfaceChanged handling", e);
                }
            }
        });
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "Surface destroyed");

        synchronized (sThreadManager) {
            // Stop render thread
            if (mRenderThread != null) {
                sThreadManager.threadExiting(mRenderThread);
                mRenderThread.requestExitAndWait();
                mRenderThread = null;
                mIsInitialized = false;
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
        mRenderThread.setRenderMode(mode);
    }

    @Override
    public void onRenderPause() {
        mRenderPaused = true;
        if (mRenderThread != null) {
            mRenderThread.onPause();
        }
    }

    @Override
    public void onRenderResume() {
        mRenderPaused = false;
        if (mRenderThread != null) {
            mRenderThread.onResume();
        }
    }

    @Override
    protected void onVisibilityChanged(View changedView, int visibility) {
        super.onVisibilityChanged(changedView, visibility);
        if (visibility == View.VISIBLE) {
            onRenderResume();
        } else {
            onRenderPause();
        }
    }

    /**
     * Queue a Runnable to be executed on the Vulkan render thread.
     */
    public void queueEvent(Runnable runnable) {
        mRenderThread.queueEvent(runnable);
    }

    /**
     * This method is used as part of the View class and is not normally
     * called or subclassed by clients of GLSurfaceView.
     */
    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        if (LOG_ATTACH_DETACH) {
            Log.d(TAG, "onAttachedToWindow reattach =" + mDetached);
        }
        if (mDetached) {
            int renderMode = RENDERMODE_CONTINUOUSLY;
            if (mRenderThread != null) {
                renderMode = mRenderThread.getRenderMode();
            }
            mRenderThread = new RenderThread(mThisWeakRef);
            if (renderMode != RENDERMODE_CONTINUOUSLY) {
                mRenderThread.setRenderMode(renderMode);
            }
            mRenderThread.start();
        }
        mDetached = false;
    }
    @Override
    protected void onDetachedFromWindow() {
        if (LOG_ATTACH_DETACH) {
            Log.d(TAG, "onDetachedFromWindow");
        }
        if (mRenderThread != null) {
            mRenderThread.requestExitAndWait();
        }
        mDetached = true;
        super.onDetachedFromWindow();
    }

    /**
     * Dedicated render thread for Vulkan rendering.
     * Mimics GLSurfaceView's render thread behavior.
     */
    private class RenderThread extends Thread {
        private WeakReference<AxmolSurfaceViewVK> mSurfaceViewWeakRef;
        private volatile boolean mShouldExit = false;
        private volatile boolean mExited = false;
        private volatile boolean mRequestRedraw = false;
        private volatile boolean mPaused = false;
        private int mRenderMode = RENDERMODE_CONTINUOUSLY;
        private final BlockingQueue<Runnable> mEventQueue = new LinkedBlockingQueue<>();
        private final Object mThreadLock = new Object();

        public RenderThread(WeakReference<AxmolSurfaceViewVK> surfaceViewWeakRef) {
            super();
            mSurfaceViewWeakRef = surfaceViewWeakRef;
        }

        @Override
        public void run() {
            setName("RenderThread " + getId());
            try {
                sThreadManager.threadStarting(mRenderThread);
                guardedRun();
            } catch (InterruptedException e) {
                // fall thru and exit normally
            } finally {
                sThreadManager.threadExiting(this);
            }
        }

        private void guardedRun() throws InterruptedException {
            // Set thread priority for better rendering performance
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_DISPLAY);

            try {
                // Main render loop
                while (!mShouldExit) {
                    // Check if rendering is paused
                    if (mPaused || mRenderPaused) {
                        synchronized (mThreadLock) {
                            try {
                                mThreadLock.wait(100); // Sleep longer when paused
                            } catch (InterruptedException e) {
                                // Thread interrupted
                                Thread.currentThread().interrupt();
                                break;
                            }
                        }
                        continue;
                    }

                    // Process pending events
                    processEvents();

                    // Render frame if initialized
                    if (mIsInitialized && !mPaused && !mRenderPaused) {
                        mPlayer.onDrawFrame();
                    }

                    // Handle render mode: wait if in WHEN_DIRTY mode
                    if (mRenderMode == RENDERMODE_WHEN_DIRTY && !mRequestRedraw) {
                        synchronized (mThreadLock) {
                            try {
                                mThreadLock.wait(); // Wait until redraw is requested
                            } catch (InterruptedException e) {
                                Thread.currentThread().interrupt();
                                break;
                            }
                        }
                    }
                    mRequestRedraw = false;
                }
            } finally {
                // Mark thread as exited
                mExited = true;

                // Cleanup resources
                cleanup();
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

        private void cleanup() {
            // Notify native layer that surface is destroyed
            if (mIsInitialized) {
                // We might need a native method for Vulkan cleanup
                // AxmolPlayer.nativeOnSurfaceDestroyed();
                Log.d(TAG, "Vulkan surface cleanup needed");
            }

            // Clear event queue
            mEventQueue.clear();
            Log.d(TAG, "RenderThread cleanup completed");
        }

        public void requestExitAndWait() {
            mShouldExit = true;
            synchronized (mThreadLock) {
                mThreadLock.notifyAll();
            }

            try {
                join(2000); // Wait up to 2 seconds
                if (isAlive()) {
                    Log.w(TAG, "RenderThread did not exit gracefully, interrupting");
                    interrupt();
                }
            } catch (InterruptedException e) {
                Log.w(TAG, "Interrupted while waiting for render thread to exit");
            }
        }

        public void requestRedraw() {
            mRequestRedraw = true;
            synchronized (mThreadLock) {
                mThreadLock.notifyAll();
            }
        }

        public void onPause() {
            mPaused = true;
        }

        public void onResume() {
            mPaused = false;
            requestRedraw(); // Request redraw when resuming
        }

        public void setRenderMode(int mode) {
            synchronized (mThreadLock) {
                if (mode == RENDERMODE_WHEN_DIRTY) {
                    mRenderMode = RENDERMODE_WHEN_DIRTY;
                } else {
                    mRenderMode = RENDERMODE_CONTINUOUSLY;
                    mThreadLock.notifyAll(); // Wake up thread if waiting
                }
            }
        }

        public int getRenderMode() {
            return mRenderMode;
        }

        public void queueEvent(Runnable runnable) {
            if (mExited) {
                Log.w(TAG, "RenderThread already exited, discarding event");
                return;
            }

            mEventQueue.offer(runnable);
            // Wake up thread if waiting
            synchronized (mThreadLock) {
                mThreadLock.notifyAll();
            }
        }

        public boolean hasExited() {
            return mExited;
        }
    }

    private final WeakReference<AxmolSurfaceViewVK> mThisWeakRef =
        new WeakReference<AxmolSurfaceViewVK>(this);

    /**
     * Simplified thread manager for Vulkan render threads.
     * Currently manages thread lifecycle tracking for potential future extension.
     */
    private static class RenderThreadManager {
        private static final String TAG = "RenderThreadManager";
        private final AtomicInteger mThreadCounter = new AtomicInteger(0);

        /**
         * Called when a render thread is starting.
         * Assigns a unique ID to the thread.
         */
        public synchronized void threadStarting(RenderThread thread) {
            // For future extension: track active threamSurfaceLockds, assign priorities, etc.
            int threadId = mThreadCounter.incrementAndGet();
            Log.d(TAG, "Vulkan render thread starting, ID: " + threadId);
        }

        /**
         * Called when a render thread is exiting.
         */
        public synchronized void threadExiting(RenderThread thread) {
            Log.d(TAG, "Vulkan render thread exiting");
            // For future extension: cleanup thread resources, notify other threads, etc.
            if (thread.hasExited()) {
                Log.d(TAG, "Thread already marked as exited");
            }
        }

        /**
         * Release Vulkan resources if needed.
         * This would be called when all Vulkan threads need to release shared resources.
         */
        public synchronized void releaseVulkanContextLocked() {
            // For future extension: if we implement shared Vulkan resources
            Log.d(TAG, "Releasing Vulkan context (placeholder)");
        }
    }
}
