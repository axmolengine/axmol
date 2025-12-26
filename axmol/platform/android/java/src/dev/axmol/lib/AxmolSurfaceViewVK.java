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
 *
 * Key points:
 * - No UI-thread surface-initialized flag; surface state is kept inside RenderThread.
 * - All native/Vulkan calls are dispatched to the render thread via queueEvent.
 * - Synchronization for thread creation/destruction uses sThreadManager (shared monitor).
 */
public class AxmolSurfaceViewVK extends SurfaceView implements AxmolRenderHost, SurfaceHolder.Callback2 {
    private static final String TAG = "AxmolSurfaceViewVK";

    private static final boolean LOG_ATTACH_DETACH = true;

    // Thread manager for coordinating multiple Vulkan render threads (if needed)
    private static final RenderThreadManager sThreadManager = new RenderThreadManager();

    // Render thread instance (owned by this view)
    private RenderThread mRenderThread;

    // Player that implements native hooks (must be provided)
    private AxmolPlayer mPlayer;

    // Rendering mode control (UI-visible flag to request pause/resume)
    private volatile boolean mRenderPaused = false;

    // Track detach/attach state similar to GLSurfaceView
    private boolean mDetached;

    private final WeakReference<AxmolSurfaceViewVK> mThisWeakRef =
        new WeakReference<AxmolSurfaceViewVK>(this);

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
        // Optionally set Z-order if needed
        setZOrderOnTop(true);
        setKeepScreenOn(true);

        // Start render thread eagerly (GLSurfaceView starts thread when setRenderer is called;
        // here we start a render thread to accept events)
        synchronized (sThreadManager) {
            if (mRenderThread == null) {
                mRenderThread = new RenderThread(mThisWeakRef);
                mRenderThread.start();
            }
        }
    }

    // -------------------------
    // SurfaceHolder callbacks
    // -------------------------
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");

        final Surface surface = holder.getSurface();
        if (surface == null) {
            Log.w(TAG, "surfaceCreated: holder.getSurface() == null");
            return;
        }

        // Synchronize on shared thread manager to mirror GLSurfaceView behavior
        synchronized (sThreadManager) {
            // Ensure render thread exists and is running
            if (mRenderThread == null || mRenderThread.hasExited()) {
                mRenderThread = new RenderThread(mThisWeakRef);
                mRenderThread.start();
            }

            // Dispatch surfaceCreated to render thread (native creation must run on render thread)
            final RenderThread rt = mRenderThread;
            rt.queueEvent(new Runnable() {
                @Override
                public void run() {
                    rt.handleSurfaceCreated(surface);
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

        if (rt == null || rt.hasExited()) {
            Log.w(TAG, "surfaceChanged: render thread not ready; resize will be handled after create");
            return;
        }

        // Dispatch resize to render thread so swapchain recreation happens on render thread
        rt.queueEvent(new Runnable() {
            @Override
            public void run() {
                rt.handleSurfaceChanged(width, height);
            }
        });
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");

        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                final RenderThread rt = mRenderThread;

                // Ask render thread to perform native cleanup on its own thread
                rt.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        rt.handleSurfaceDestroyed();
                    }
                });

                // Request thread exit and wait for cleanup to complete
                sThreadManager.threadExiting(rt);
                rt.requestExitAndWait();
                mRenderThread = null;
            }
        }
    }

    @Override
    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        // Request a redraw if needed
        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                mRenderThread.requestRedraw();
            }
        }
    }

    // -------------------------
    // AxmolRenderHost methods
    // -------------------------
    @Override
    public void configureRenderMode(int mode) {
        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                mRenderThread.setRenderMode(mode);
            }
        }
    }

    @Override
    public void onRenderPause() {
        mRenderPaused = true;
        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                mRenderThread.onPause();
            }
        }
    }

    @Override
    public void onRenderResume() {
        mRenderPaused = false;
        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                mRenderThread.onResume();
            }
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
     * If the render thread is not available, the runnable will be dropped with a warning.
     */
    public void queueEvent(Runnable runnable) {
        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                mRenderThread.queueEvent(runnable);
            } else {
                Log.w(TAG, "queueEvent: render thread is null, dropping event");
            }
        }
    }

    // -------------------------
    // Attach / detach handling
    // -------------------------
    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        if (LOG_ATTACH_DETACH) {
            Log.d(TAG, "onAttachedToWindow reattach =" + mDetached);
        }
        if (mDetached) {
            synchronized (sThreadManager) {
                int renderMode = RenderThread.RENDERMODE_CONTINUOUSLY;
                if (mRenderThread != null) {
                    renderMode = mRenderThread.getRenderMode();
                }
                mRenderThread = new RenderThread(mThisWeakRef);
                if (renderMode != RenderThread.RENDERMODE_CONTINUOUSLY) {
                    mRenderThread.setRenderMode(renderMode);
                }
                mRenderThread.start();
            }
        }
        mDetached = false;
    }

    @Override
    protected void onDetachedFromWindow() {
        if (LOG_ATTACH_DETACH) {
            Log.d(TAG, "onDetachedFromWindow");
        }
        synchronized (sThreadManager) {
            if (mRenderThread != null) {
                mRenderThread.requestExitAndWait();
                mRenderThread = null;
            }
        }
        mDetached = true;
        super.onDetachedFromWindow();
    }

    // -------------------------
    // Internal class: RenderThread implementation
    // -------------------------
    private class RenderThread extends Thread {
        private WeakReference<AxmolSurfaceViewVK> mSurfaceViewWeakRef;
        private final BlockingQueue<Runnable> mEventQueue = new LinkedBlockingQueue<>();
        private final Object mThreadLock = new Object();

        // Thread control flags
        private volatile boolean mShouldExit = false;
        private volatile boolean mExited = false;
        private volatile boolean mRequestRedraw = false;
        private volatile boolean mPaused = false;

        // Render mode constants (mirror GLSurfaceView)
        public static final int RENDERMODE_WHEN_DIRTY = 0;
        public static final int RENDERMODE_CONTINUOUSLY = 1;
        private int mRenderMode = RENDERMODE_CONTINUOUSLY;

        // Surface / initialization state (kept on render thread)
        private boolean mSurfaceInitialized = false;
        private Surface mCurrentSurface = null;

        public RenderThread(WeakReference<AxmolSurfaceViewVK> surfaceViewWeakRef) {
            super();
            mSurfaceViewWeakRef = surfaceViewWeakRef;
        }

        @Override
        public void run() {
            setName("RenderThread " + getId());
            try {
                sThreadManager.threadStarting(this);
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
                    // Process pending events first
                    processEvents();

                    // If paused, wait until resumed
                    if (mPaused || mRenderPaused) {
                        synchronized (mThreadLock) {
                            try {
                                mThreadLock.wait(100); // Sleep longer when paused
                            } catch (InterruptedException e) {
                                Thread.currentThread().interrupt();
                                break;
                            }
                        }
                        continue;
                    }

                    // Render frame if initialized
                    if (mSurfaceInitialized && !mPaused && !mRenderPaused) {
                        try {
                            mPlayer.onDrawFrame();
                        } catch (Throwable t) {
                            Log.e(TAG, "Exception in onDrawFrame", t);
                        }
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
                    } else {
                        // In continuous mode yield to avoid busy loop
                        Thread.yield();
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
            // Notify native layer that surface is destroyed if needed
            if (mSurfaceInitialized) {
                try {
                } catch (Throwable t) {
                    Log.e(TAG, "Exception during nativeOnSurfaceDestroyed in cleanup", t);
                }
                mSurfaceInitialized = false;
                mCurrentSurface = null;
            }

            // Clear event queue
            mEventQueue.clear();
            Log.d(TAG, "RenderThread cleanup completed");
        }

        // -------------------------
        // Surface handlers (run on render thread)
        // -------------------------
        void handleSurfaceCreated(Surface surface) {
            // Defensive: if already initialized with same surface, skip
            if (mSurfaceInitialized && mCurrentSurface == surface) {
                Log.d(TAG, "handleSurfaceCreated: already initialized with same surface");
                return;
            }

            // If previously initialized with another surface, destroy it first
            if (mSurfaceInitialized) {
                try {
                    mPlayer.onSurfaceDestroyed();
                } catch (Throwable t) {
                    Log.e(TAG, "Exception during nativeOnSurfaceDestroyed before reinit", t);
                }
                mSurfaceInitialized = false;
                mCurrentSurface = null;
            }

            // Call into player/native to create VkSurfaceKHR using the provided Surface.
            try {
                // Use mPlayer.onSurfaceCreated(surface) if your Java->native path is implemented there.
                // If nativeOnSurfaceCreated exists and returns boolean, you can call it instead.
                mPlayer.onSurfaceCreated(surface);
                // If no exception thrown, mark as initialized. If your native call can fail,
                // adapt to check return value or throw on failure.
                mSurfaceInitialized = true;
                mCurrentSurface = surface;
                Log.d(TAG, "handleSurfaceCreated: native surface created successfully");
            } catch (Throwable t) {
                Log.e(TAG, "Exception during onSurfaceCreated", t);
                mSurfaceInitialized = false;
                mCurrentSurface = null;
            }
        }

        void handleSurfaceChanged(int width, int height) {
            if (!mSurfaceInitialized) {
                Log.w(TAG, "handleSurfaceChanged: surface not initialized yet");
                return;
            }
            try {
                AxmolPlayer.nativeOnSurfaceChanged(width, height);
            } catch (Throwable t) {
                Log.e(TAG, "Exception during nativeOnSurfaceChanged", t);
            }
        }

        void handleSurfaceDestroyed() {
            if (!mSurfaceInitialized) {
                Log.d(TAG, "handleSurfaceDestroyed: nothing to destroy");
                return;
            }
            try {
                mPlayer.onSurfaceDestroyed();
            } catch (Throwable t) {
                Log.e(TAG, "Exception during nativeOnSurfaceDestroyed", t);
            } finally {
                mSurfaceInitialized = false;
                mCurrentSurface = null;
            }
        }

        // -------------------------
        // Control methods (can be called from UI thread)
        // -------------------------
        public void requestExitAndWait() {
            // don't call this from GLThread thread or it is a guaranteed
            // deadlock!
            synchronized(sThreadManager) {
                mShouldExit = true;
                sThreadManager.notifyAll();
                while (! mExited) {
                    try {
                        sThreadManager.wait();
                    } catch (InterruptedException ex) {
                        Thread.currentThread().interrupt();
                    }
                }
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

    /** Internal class: RenderThreadManager
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
            int threadId = mThreadCounter.incrementAndGet();
            Log.d(TAG, "Vulkan render thread starting, ID: " + threadId);
        }

        /**
         * Called when a render thread is exiting.
         */
        public synchronized void threadExiting(RenderThread thread) {
            Log.d(TAG, "Vulkan render thread exiting");
            if (thread.hasExited()) {
                Log.d(TAG, "Thread already marked as exited");
            }
        }

        /**
         * Release Vulkan resources if needed.
         * This would be called when all Vulkan threads need to release shared resources.
         */
        public synchronized void releaseVulkanContextLocked() {
            Log.d(TAG, "Releasing Vulkan context (placeholder)");
        }
    }
}
