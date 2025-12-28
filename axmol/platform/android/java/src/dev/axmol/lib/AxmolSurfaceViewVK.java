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

import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.util.Log;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
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
    private static final boolean LOG_THREADS = false;
    private final static boolean LOG_PAUSE_RESUME = false;
    private final static boolean LOG_SURFACE = false;
    private final static boolean LOG_RENDERER = false;
    private final static boolean LOG_RENDERER_DRAW_FRAME = false;

    // Thread manager for coordinating multiple Vulkan render threads (if needed)
    private static final RenderThreadManager sThreadManager = new RenderThreadManager();

    // Render thread instance (owned by this view)
    private RenderThread mRenderThread;

    // Player that implements native hooks (must be provided)
    private AxmolPlayer mPlayer;

    // Track detach/attach state similar to GLSurfaceView
    private boolean mDetached = false;

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

    @Override
    public void setRenderMode(int mode) {
        if (mRenderThread != null) {
            mRenderThread.setRenderMode(mode);
        }
    }

    @Override
    public int getRenderMode() {
        return mRenderThread != null ? mRenderThread.getRenderMode() : RENDERMODE_CONTINUOUSLY;
    }

    public void requestRender() {
        if (mRenderThread != null) {
            mRenderThread.requestRender();
        }
    }

    // -------------------------
    // SurfaceHolder callbacks
    // -------------------------
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (mRenderThread != null) {
            mRenderThread.surfaceCreated(holder);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mRenderThread != null) {
            mRenderThread.surfaceDestroyed();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        if (mRenderThread != null) {
            mRenderThread.onWindowResize(w, h);
        }
    }

    @Override
    public void surfaceRedrawNeededAsync(SurfaceHolder holder, Runnable finishDrawing) {
        if (mRenderThread != null) {
            mRenderThread.requestRenderAndNotify(finishDrawing);
        }
    }

    @Deprecated
    @Override
    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        // Since we are part of the framework we know only surfaceRedrawNeededAsync
        // will be called.
    }

    @Override
    public void onPause() {
        if (mRenderThread != null) mRenderThread.onPause();
    }

    @Override
    public void onResume() {
        if (mRenderThread != null) mRenderThread.onResume();
    }

    /**
     * Queue a Runnable to be executed on the Vulkan render thread.
     * If the render thread is not available, the runnable will be dropped with a warning.
     */
    public void queueEvent(Runnable runnable) {
        if (mRenderThread != null) {
            mRenderThread.queueEvent(runnable);
        } else {
            Log.w(TAG, "RenderThread is null, dropping event");
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

        // Thread control flags
        private boolean mShouldExit;
        private boolean mExited;
        private boolean mRequestPaused;
        private boolean mPaused;
        private boolean mHasSurface;
        private boolean mWaitingForSurface;
        private boolean mShouldRelease;

        private int mWidth;
        private int mHeight;
        private int mRenderMode = RENDERMODE_CONTINUOUSLY;
        private boolean mRequestRender;
        private boolean mWantRenderNotification;
        private boolean mRenderComplete;
        private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();
        private Runnable mFinishDrawingRunnable = null;

        /**
         * Set once at thread construction time, nulled out when the parent view is garbage
         * called. This weak reference allows the VKSurfaceView to be garbage collected while
         * the RenderThread is still alive.
         */
        private WeakReference<AxmolSurfaceViewVK> mSurfaceViewWeakRef;

        public RenderThread(WeakReference<AxmolSurfaceViewVK> surfaceViewWeakRef) {
            super();

            mWidth = 0;
            mHeight = 0;
            mRequestRender = true;
            mRenderMode = RENDERMODE_CONTINUOUSLY;
            mWantRenderNotification = false;
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
            boolean doRenderNotification = false;
            Runnable event = null;

            try {
                while (true) {
                    synchronized (sThreadManager) {
                        while (true) {
                            if (mShouldExit) {
                                return;
                            }

                            // Process queued events first
                            if (!mEventQueue.isEmpty()) {
                                event = mEventQueue.remove(0);
                                break;
                            }

                            // Update pause state
                            if (mPaused != mRequestPaused) {
                                mPaused = mRequestPaused;
                                sThreadManager.notifyAll();
                            }

                            // Update surface waiting state
                            if (mHasSurface != mWaitingForSurface) {
                                mWaitingForSurface = !mHasSurface;
                                sThreadManager.notifyAll();
                            }

                            // Check if we should release resources
                            if (mShouldRelease) {
                                mShouldRelease = false;
                                sThreadManager.notifyAll();
                            }

                            // Determine if we should render
                            boolean readyToRender = mHasSurface && (mWidth > 0) && (mHeight > 0) &&
                                !mPaused && (mRequestRender || (mRenderMode == RENDERMODE_CONTINUOUSLY));

                            if (readyToRender) {
                                // consume request render
                                mRequestRender = false;

                                // If UI requested a render-notify, convert to local flag
                                if (mWantRenderNotification) {
                                    mWantRenderNotification = false;
                                    doRenderNotification = true;
                                }

                                sThreadManager.notifyAll();
                                break; // Exit wait loop to render
                            }

                            // Wait for next event or state change
                            sThreadManager.wait();
                        }
                    }

                    // Process queued event (outside of sThreadManager lock)
                    if (event != null) {
                        try {
                            event.run();
                        } catch (RuntimeException e) {
                            Log.e(TAG, "Event runnable threw", e);
                        }
                        event = null;
                        continue;
                    }

                    // Perform rendering (do not hold sThreadManager while calling into player)
                    if (mHasSurface && (mWidth > 0) && (mHeight > 0) && !mPaused) {
                        try {
                            AxmolSurfaceViewVK view = mSurfaceViewWeakRef.get();
                            AxmolPlayer player = (view != null) ? view.mPlayer : null;
                            if (player != null) {
                                player.onDrawFrame();
                            } else {
                                Log.w(TAG, "Skipping onDrawFrame: view or player is null");
                            }
                        } catch (RuntimeException e) {
                            Log.e(TAG, "Exception during frame rendering", e);
                        }

                        // After rendering, execute finishDrawing runnable if present.
                        Runnable finish = null;
                        synchronized (sThreadManager) {
                            finish = mFinishDrawingRunnable;
                            mFinishDrawingRunnable = null;
                        }
                        if (finish != null) {
                            try {
                                finish.run();
                            } catch (RuntimeException e) {
                                Log.e(TAG, "finishDrawing runnable threw", e);
                            }
                        }

                        // Notify render completion if requested (must notify while holding lock)
                        if (doRenderNotification) {
                            synchronized (sThreadManager) {
                                mRenderComplete = true;
                                sThreadManager.notifyAll();
                            }
                            doRenderNotification = false;
                        }
                    }
                }
            } finally {
                // Clear event queue and mark exited
                synchronized (sThreadManager) {
                    mExited = true;
                    sThreadManager.notifyAll();
                }
                mEventQueue.clear();
                Log.d(TAG, "RenderThread cleanup completed");
            }
        }

        private boolean readyToDraw() {
            return (!mPaused) && mHasSurface
                && (mWidth > 0) && (mHeight > 0)
                && (mRequestRender || (mRenderMode == RENDERMODE_CONTINUOUSLY));
        }

        // -------------------------
        // Control methods (can be called from UI thread)
        // -------------------------
        public void setRenderMode(int renderMode) {
            if (!((RENDERMODE_WHEN_DIRTY <= renderMode) && (renderMode <= RENDERMODE_CONTINUOUSLY))) {
                throw new IllegalArgumentException("renderMode");
            }
            synchronized (sThreadManager) {
                mRenderMode = renderMode;
                sThreadManager.notifyAll();
            }
        }

        public int getRenderMode() {
            synchronized (sThreadManager) {
                return mRenderMode;
            }
        }

        public void requestRender() {
            synchronized (sThreadManager) {
                mRequestRender = true;
                sThreadManager.notifyAll();
            }
        }

        /**
         * Request a render and notify caller when the next frame is finished.
         * finishDrawing will be executed on the render thread after the frame is drawn.
         */
        public void requestRenderAndNotify(Runnable finishDrawing) {
            synchronized (sThreadManager) {
                // reentrancy guard: if called from render thread, ignore
                if (Thread.currentThread() == this) {
                    return;
                }
                mWantRenderNotification = true;
                mRequestRender = true;
                mRenderComplete = false;
                final Runnable oldCallback = mFinishDrawingRunnable;
                mFinishDrawingRunnable = () -> {
                    if (oldCallback != null) {
                        oldCallback.run();
                    }
                    if (finishDrawing != null) {
                        finishDrawing.run();
                    }
                };
                sThreadManager.notifyAll();
            }
        }

        /**
         * Called from UI thread when SurfaceHolder.surfaceCreated is invoked.
         * We queue an event to the render thread; the runnable will set mHasSurface
         * and call player's onSurfaceCreated on the render thread.
         */
        public void surfaceCreated(final SurfaceHolder holder) {
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    synchronized (sThreadManager) {
                        mHasSurface = true;
                        mRequestRender = true;
                        sThreadManager.notifyAll(); // must notify while holding lock
                    }
                    AxmolSurfaceViewVK view = mSurfaceViewWeakRef.get();
                    if (view != null && view.mPlayer != null) {
                        try {
                            view.mPlayer.onSurfaceCreated(holder.getSurface());
                        } catch (RuntimeException e) {
                            Log.e(TAG, "onSurfaceCreated threw", e);
                        }
                    }
                }
            });
        }

        /**
         * Called from UI thread when SurfaceHolder.surfaceDestroyed is invoked.
         * We queue an event to the render thread; the runnable will clear mHasSurface
         * and call player's onSurfaceDestroyed on the render thread.
         */
        public void surfaceDestroyed() {
            queueEvent(new Runnable() {
                @Override
                public void run() {
                    synchronized (sThreadManager) {
                        mHasSurface = false;
                        mRequestRender = false;
                        sThreadManager.notifyAll(); // must notify while holding lock
                    }
                }
            });
        }

        public void onPause() {
            synchronized (sThreadManager) {
                if (LOG_PAUSE_RESUME) {
                    Log.i("RenderThread", "onPause tid=" + getId());
                }
                mRequestPaused = true;
                sThreadManager.notifyAll();
                while ((!mExited) && (!mPaused)) {
                    if (LOG_PAUSE_RESUME) {
                        Log.i("Main thread", "onPause waiting for mPaused.");
                    }
                    try {
                        sThreadManager.wait();
                    } catch (InterruptedException ex) {
                        Thread.currentThread().interrupt();
                    }
                }
            }
        }

        public void onResume() {
            synchronized (sThreadManager) {
                if (LOG_PAUSE_RESUME) {
                    Log.i("RenderThread", "onResume tid=" + getId());
                }
                mRequestPaused = false;
                mRequestRender = true;
                mRenderComplete = false;
                sThreadManager.notifyAll();
                while ((!mExited) && mPaused && (!mRenderComplete)) {
                    if (LOG_PAUSE_RESUME) {
                        Log.i("Main thread", "onResume waiting for !mPaused.");
                    }
                    try {
                        sThreadManager.wait();
                    } catch (InterruptedException ex) {
                        Thread.currentThread().interrupt();
                    }
                }
            }
        }

        /**
         * Handle window resize from UI thread. We queue a runnable to the render thread
         * which updates sizes and calls player's onSurfaceChanged. The UI thread waits
         * for render-thread-side handling to complete, with a timeout to avoid indefinite blocking.
         */
        public void onWindowResize(final int w, final int h) {
            // Update desired size and request a render from the UI thread side
            synchronized (sThreadManager) {
                mWidth = w;
                mHeight = h;
                mRequestRender = true;
                mRenderComplete = false;
            }

            // If we're already on the render thread, just return; next loop will pick up changes.
            if (Thread.currentThread() == this) {
                return;
            }

            queueEvent(new Runnable() {
                @Override
                public void run() {
                    try {
                        synchronized (sThreadManager) {
                            mWidth = w;
                            mHeight = h;
                            // notify any waiters that size changed
                            sThreadManager.notifyAll();
                        }
                        AxmolSurfaceViewVK view = mSurfaceViewWeakRef.get();
                        if (view != null && view.mPlayer != null) {
                            try {
                                view.mPlayer.onSurfaceChanged(w, h);
                            } catch (RuntimeException e) {
                                Log.e(TAG, "onSurfaceChanged threw", e);
                            }
                        }
                    } finally {
                        synchronized (sThreadManager) {
                            mRenderComplete = true;
                            sThreadManager.notifyAll();
                        }
                    }
                }
            });

            // Wait for the render thread to process the resize and render a frame.
            // If the render thread is paused, exited, or not ready to draw, don't block indefinitely.
            long start = System.currentTimeMillis();
            long timeoutMs = 2000;
            synchronized (sThreadManager) {
                while (!mExited && !mPaused && !mRenderComplete && readyToDraw()) {
                    if (LOG_SURFACE) {
                        Log.i(TAG, "onWindowResize waiting for render complete from tid=" + getId());
                    }
                    long elapsed = System.currentTimeMillis() - start;
                    long waitMs = timeoutMs - elapsed;
                    if (waitMs <= 0) break;
                    try {
                        sThreadManager.wait(waitMs);
                    } catch (InterruptedException ex) {
                        Thread.currentThread().interrupt();
                        break;
                    }
                }
            }
        }

        public void requestExitAndWait() {
            // don't call this from RenderThread thread or it is a guaranteed deadlock!
            synchronized (sThreadManager) {
                mShouldExit = true;
                sThreadManager.notifyAll();
                while (!mExited) {
                    try {
                        sThreadManager.wait();
                    } catch (InterruptedException ex) {
                        Thread.currentThread().interrupt();
                    }
                }
            }
        }

        public void queueEvent(Runnable r) {
            if (r == null) {
                throw new IllegalArgumentException("r must not be null");
            }
            synchronized (sThreadManager) {
                mEventQueue.add(r);
                sThreadManager.notifyAll();
            }
        }

        public boolean hasExited() {
            return mExited;
        }
    }

    /**
     * Internal class: RenderThreadManager
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
    }
}
