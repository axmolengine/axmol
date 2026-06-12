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

import android.app.Activity;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.view.ActionMode;

import java.lang.ref.WeakReference;
import java.util.concurrent.CountDownLatch;

public class AxmolPlayer extends FrameLayout {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final String TAG = AxmolPlayer.class.getSimpleName();

//    enum class EditAction : int
//    {
//        Copy = 0, /**< Copy the current selection to the clipboard. */
//            Cut,      /**< Copy the current selection to the clipboard and delete it. */
//            Paste,    /**< Insert clipboard contents at the caret. */
//            SelectAll /**< Select all editable content in the current field. */
//    };

    // match with native enum class EditAction in axmol/base/InputDelegate.h
    private final static int EDIT_ACTION_COPY = 0;
    private final static int EDIT_ACTION_CUT = 1;
    private final static int EDIT_ACTION_PASTE = 2;
    private final static int EDIT_ACTION_SELECT_ALL = 3;

    private final static int IMM_OPEN_IME_KEYBOARD = 1;
    private final static int IMM_CLOSE_IME_KEYBOARD = 2;
    private static final int IMM_SHOW_CONTEXT_MENU = 4;
    private static final int IMM_HIDE_CONTEXT_MENU = 5;

    // ===========================================================
    // Constants
    // ===========================================================

    private final static long NANOSECONDSPERSECOND = 1000000000L;
    private final static long NANOSECONDSPERMICROSECOND = 1000000L;

    // The final animation interval which is used in 'onDrawFrame'
    private static long sAnimationInterval = (long) (1.0f / 60f * AxmolPlayer.NANOSECONDSPERSECOND);
    private static long FPS_CONTROL_THRESHOLD = (long) (1.0f / 1200.0f * AxmolPlayer.NANOSECONDSPERSECOND);

    // ===========================================================
    // Fields
    // ===========================================================
    private long mLastTickInNanoSeconds;

    public static int sScreenWidth = 960;
    public static int sScreenHeight = 640;

    private static Handler sHandler;
    private static WeakReference<AxmolPlayer> sInstance;

    private static boolean sNativeInitialized = false;

    private AxmolRenderHost mRenderHost; // GLSurfaceView or SurfaceView

    private boolean mSoftKeyboardShown = false;
    private boolean mHideNativeInputBar = true;

    private boolean mEnableForceDoLayout = false;

    private boolean mSurfaceCreated = false;

    private CountDownLatch mNativePauseComplete;

    private WebViewHelper mWebViewHelper = null;
    private EditBoxHelper mEditBoxHelper = null;

    private int mLastSurfaceWidth = 0;
    private int mLastSurfaceHeight = 0;

    // A reference to the active popup menu so we can dismiss it programmatically
    private static ActionMode sCurrentActionMode = null;

    // ===========================================================
    // High-Performance Touch Input Caching
    // ===========================================================
    private static final int MAX_TOUCHES = 10;

    // Pre-allocated arrays (Zero GC during touch events)
    private final int[] mTouchIds = new int[MAX_TOUCHES];
    private final float[] mTouchXs = new float[MAX_TOUCHES];
    private final float[] mTouchYs = new float[MAX_TOUCHES];
    private final float[] mTouchPressures = new float[MAX_TOUCHES];

    /**
     * Data wrapper containing text component capabilities snapshot from C++ thread.
     */
    private static class EditMenuParams {
        float x;
        float y;
        boolean hasText;
        boolean hasSelection;
        boolean readOnly;
    }

    @SuppressWarnings("unused")
    public boolean isSoftKeyboardShown() {
        return mSoftKeyboardShown;
    }

    public void setSoftKeyboardShown(boolean softKeyboardShown) {
        this.mSoftKeyboardShown = softKeyboardShown;
    }

    public void setEnableForceDoLayout(boolean flag) {
        mEnableForceDoLayout = flag;
    }

    public boolean isHideNativeInputBar() {
        return mHideNativeInputBar;
    }

    @SuppressWarnings("unused")
    public void setHideNativeInputBar(boolean flag) {
        mHideNativeInputBar = flag;
    }

    // ===========================================================
    // Constructors
    // ===========================================================

    public AxmolPlayer(final Context ctx) {
        super(ctx);
        initView(ctx);

        sInstance = new WeakReference<AxmolPlayer>(this);
    }

    // ===========================================================
    // Initialization
    // ===========================================================

    protected void initView(Context ctx) {

        boolean isVulkan = AxmolEngine.nativeGetRenderAPI() == AxmolEngine.RENDER_API_VK;

        // FrameLayout
        LayoutParams frameParams = new LayoutParams(
            LayoutParams.MATCH_PARENT,
            LayoutParams.MATCH_PARENT
        );
        setLayoutParams(frameParams);

        // Handler for IME open/close
        sHandler = new Handler(msg -> {
            InputMethodManager imm = (InputMethodManager) ctx.getSystemService(Context.INPUT_METHOD_SERVICE);
            View hostView = (View) mRenderHost;
            switch (msg.what) {
                case IMM_OPEN_IME_KEYBOARD:
                    hostView.setFocusable(true);
                    hostView.setFocusableInTouchMode(true);
                    hostView.requestFocus();

                    if (imm != null) {
                        // Target the showSoftInput directly onto the active rendering surface view
                        imm.showSoftInput(hostView, InputMethodManager.SHOW_IMPLICIT);
                        Log.d(TAG, "showSoftInput successfully bounded to mRenderHost surface");
                    }
                    break;
                case IMM_CLOSE_IME_KEYBOARD:
                    if (imm != null) {
                        imm.hideSoftInputFromWindow(getWindowToken(), 0);
                        Log.d(TAG, "HideSoftInput from AxmolPlayer");
                    }
                    if (ctx instanceof AxmolActivity) {
                        ((AxmolActivity) ctx).hideVirtualButton();
                    }
                    break;
                case IMM_SHOW_CONTEXT_MENU:
                    if (msg.obj instanceof EditMenuParams) {
                        // Safely handle UI drawing on the synchronized Android Main UI thread
                        showContextMenuOnUIThread((EditMenuParams) msg.obj);
                    }
                    break;
                case IMM_HIDE_CONTEXT_MENU:
                    // Forcefully terminate the active context text action mode session safely
                    if (sCurrentActionMode != null) {
                        try {
                            sCurrentActionMode.finish();
                            Log.d(TAG, "Native floating ActionMode successfully finished via hide handler.");
                        } catch (Exception e) {
                            // Defensive exception trap to handle edge cases where the window is already being torn down by the OS
                            Log.e(TAG, "Failed to finish sCurrentActionMode gracefully: " + e.getMessage());
                        } finally {
                            // Ensure the static reference is always decoupled to prevent memory leaks
                            sCurrentActionMode = null;
                        }
                    }
                    break;
            }
            return true;
        });

        // Decide backend
        if (isVulkan) {
            initVulkanView(ctx);
        } else {
            initGLView(ctx);
        }

        if (mWebViewHelper == null) {
            mWebViewHelper = new WebViewHelper(this);
        }

        if (mEditBoxHelper == null) {
            mEditBoxHelper = new EditBoxHelper(this);
        }
    }

    private void initGLView(Context ctx) {
        AxmolSurfaceViewGL surfaceView = new AxmolSurfaceViewGL(this);
        mRenderHost = surfaceView;
        addView(surfaceView);
        initKeyboardVisibilityTracker(surfaceView);
    }

    private void initVulkanView(Context ctx) {
        AxmolSurfaceViewVK surfaceView = new AxmolSurfaceViewVK(this);
        mRenderHost = surfaceView;
        addView(surfaceView);
        initKeyboardVisibilityTracker(surfaceView);
    }

    private void initKeyboardVisibilityTracker(View targetView) {
        // One-liner implementation completely delegating to our unified tracker component
        AxmolKeyboardTracker.register(targetView);
    }

    @SuppressWarnings("unused")
    public AxmolInputConnection createInputConnection(View targetView, EditorInfo outAttrs) {
        // Set standard text input type
        outAttrs.inputType = EditorInfo.TYPE_CLASS_TEXT | EditorInfo.TYPE_TEXT_FLAG_AUTO_CORRECT;

        // Modern IMEs require valid selection bounds to activate text input streaming.
        // Without these, the IME might assume the field is un-editable and refuse to call commitText.
        outAttrs.initialSelStart = 0;
        outAttrs.initialSelEnd = 0;
        outAttrs.initialCapsMode = 0;

        // Disable Extract Mode (fullscreen keyboard) in landscape orientation
        if (isHideNativeInputBar()) {
            outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI;
            Log.d(TAG, "IME Extract Mode disabled dynamically via hideNativeInputBar=true");
        }
        else {
            outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE;
            Log.d(TAG, "IME Extract Mode allowed via hideNativeInputBar=false");
        }

        // Return the clean custom connection channel
        return new AxmolInputConnection(targetView, false);
    }

    // ===========================================================
    // Render loop controller methods
    // ===========================================================

    @SuppressWarnings("unused")
    public static void setAnimationInterval(float interval) {
        sAnimationInterval = (long) (interval * NANOSECONDSPERSECOND);
    }

    public void onDrawFrame() {
        /*
         * Render time MUST be counted in, or the FPS will slower than appointed.
         */
        nativeRenderFrame();
        /*
         * No need to use algorithm in default(60,90,120... FPS) situation,
         * since onDrawFrame() was called by system 60 times per second by default.
         */
        if (sAnimationInterval > FPS_CONTROL_THRESHOLD) {
            final long interval = System.nanoTime() - this.mLastTickInNanoSeconds;

            if (interval < sAnimationInterval) {
                try {
                    Thread.sleep((sAnimationInterval - interval) / NANOSECONDSPERMICROSECOND);
                } catch (final Exception e) {
                }
            }

            this.mLastTickInNanoSeconds = System.nanoTime();
        }
    }

    public void onSurfaceCreated(Surface surface) {
        this.mLastTickInNanoSeconds = System.nanoTime();
        boolean isWarmStart = !mSurfaceCreated;
        mSurfaceCreated = true;
        AxmolPlayer.nativeOnSurfaceCreated(surface, sScreenWidth, sScreenHeight, isWarmStart);
        if (!sNativeInitialized) {
            sNativeInitialized = true;
        }
    }

    public void onSurfaceChanged(int width, int height) {
        if (mLastSurfaceWidth != width || mLastSurfaceHeight != height) {
            mLastSurfaceWidth = width;
            mLastSurfaceHeight = height;
            nativeOnSurfaceChanged(width, height);
        }
    }

    // ===========================================================
    // Static access
    // ===========================================================

    public static AxmolPlayer getInstance() {
        return sInstance != null ? sInstance.get() : null;
    }

    public static void queueAccelerometer(final float x, final float y, final float z, final long timestamp) {
        AxmolEngine.runOnAxmolThread(new Runnable() {
            @Override
            public void run() {
                AxmolAccelerometer.onSensorChanged(x, y, z, timestamp);
            }
        });
    }

    // ===========================================================
    // IME control
    // ===========================================================

    @SuppressWarnings("unused")
    public static void openIMEKeyboard() {
        final Message msg = Message.obtain();
        msg.what = IMM_OPEN_IME_KEYBOARD;
        sHandler.sendMessage(msg);
    }

    @SuppressWarnings("unused")
    public static void closeIMEKeyboard() {
        final Message msg = Message.obtain();
        msg.what = IMM_CLOSE_IME_KEYBOARD;
        sHandler.sendMessage(msg);
    }

    @SuppressWarnings("unused")
    public static void showContextMenu(float x, float y, boolean hasText, boolean hasSelection, boolean readOnly) {
        if (sHandler == null) return;

        EditMenuParams params = new EditMenuParams();
        params.x = x;
        params.y = y;
        params.hasText = hasText;
        params.hasSelection = hasSelection;
        params.readOnly = readOnly;

        // Route the call securely across threads from JNI worker thread to Main UI thread
        Message msg = Message.obtain();
        msg.what = IMM_SHOW_CONTEXT_MENU;
        msg.obj = params;
        sHandler.sendMessage(msg);
    }

    @SuppressWarnings("unused")
    public static void hideContextMenu() {
        if (sHandler == null) return;
        sHandler.sendEmptyMessage(IMM_HIDE_CONTEXT_MENU);
    }

    // ===========================================================
    // Touch & Key Events
    // ===========================================================

    @Override
    public boolean onTouchEvent(final MotionEvent motionEvent) {
        if (mSoftKeyboardShown) {
            InputMethodManager imm = (InputMethodManager) this.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            View view = ((Activity) this.getContext()).getCurrentFocus();
            if (null != view) {
                imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
            }
            this.requestFocus();
            mSoftKeyboardShown = false;
        }

        final int action = motionEvent.getActionMasked();
        final int pointerCount = Math.min(motionEvent.getPointerCount(), MAX_TOUCHES);

        switch (action) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN: {
                final int index = motionEvent.getActionIndex();
                final int id = motionEvent.getPointerId(index);
                final float x = motionEvent.getX(index);
                final float y = motionEvent.getY(index);
                final float pressure = motionEvent.getPressure(index); // 🔴 Extract pressure

                nativeTouchBegin(id, x, y, pressure);
                break;
            }

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP: {
                final int index = motionEvent.getActionIndex();
                final int id = motionEvent.getPointerId(index);
                final float x = motionEvent.getX(index);
                final float y = motionEvent.getY(index);
                final float pressure = motionEvent.getPressure(index);

                nativeTouchEnd(id, x, y, pressure);

                if (action == MotionEvent.ACTION_UP) {
                    performClick(); // Accessibility requirement
                }
                break;
            }

            case MotionEvent.ACTION_MOVE: {
                for (int i = 0; i < pointerCount; i++) {
                    mTouchIds[i] = motionEvent.getPointerId(i);
                    mTouchXs[i] = motionEvent.getX(i);
                    mTouchYs[i] = motionEvent.getY(i);
                    mTouchPressures[i] = motionEvent.getPressure(i);
                }
                nativeTouchesMove(mTouchIds, mTouchXs, mTouchYs, mTouchPressures, pointerCount);
                break;
            }

            case MotionEvent.ACTION_CANCEL: {
                for (int i = 0; i < pointerCount; i++) {
                    mTouchIds[i] = motionEvent.getPointerId(i);
                    mTouchXs[i] = motionEvent.getX(i);
                    mTouchYs[i] = motionEvent.getY(i);
                    mTouchPressures[i] = motionEvent.getPressure(i); // 🔴 Populate pressure
                }
                nativeTouchesCancel(mTouchIds, mTouchXs, mTouchYs, mTouchPressures, pointerCount);
                break;
            }
        }

        return true;
    }

    // Suppress lint warning only
    @Override
    public boolean performClick() {
        super.performClick();
        return true;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        if (!this.isInEditMode()) {
            sScreenWidth = w;
            sScreenHeight = h;
        }
    }

    @Override
    public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
        switch (pKeyCode) {
            case KeyEvent.KEYCODE_BACK:
            case KeyEvent.KEYCODE_MENU:
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
            case KeyEvent.KEYCODE_DPAD_UP:
            case KeyEvent.KEYCODE_DPAD_DOWN:
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE:
            case KeyEvent.KEYCODE_DPAD_CENTER:
                AxmolEngine.runOnAxmolThread(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeKeyEvent(pKeyCode, true);
                    }
                });
                return true;
            default:
                return super.onKeyDown(pKeyCode, pKeyEvent);
        }
    }

    @Override
    public boolean onKeyUp(final int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
            case KeyEvent.KEYCODE_MENU:
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
            case KeyEvent.KEYCODE_DPAD_UP:
            case KeyEvent.KEYCODE_DPAD_DOWN:
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE:
            case KeyEvent.KEYCODE_DPAD_CENTER:
                AxmolEngine.runOnAxmolThread(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeKeyEvent(keyCode, false);
                    }
                });
                return true;
            default:
                return super.onKeyUp(keyCode, event);
        }
    }

    // ===========================================================
    // Pause/Resume
    // ===========================================================

    public void onPause() {
        mRenderHost.onPause();
    }

    public void onResume() {
        mRenderHost.onResume();
    }

    public void handleOnResume() {
        mRenderHost.setRenderMode(AxmolRenderHost.RENDERMODE_CONTINUOUSLY);

        AxmolEngine.runOnAxmolThread(new Runnable() {
            @Override
            public void run() {
                if (AxmolEngine.sNativePaused) {
                    AxmolPlayer.nativeOnResume();
                    AxmolEngine.sNativePaused = false;
                }
            }
        });
    }

    public void handleOnPause() {
        mRenderHost.setRenderMode(AxmolRenderHost.RENDERMODE_WHEN_DIRTY);

        mNativePauseComplete = new CountDownLatch(1);

        AxmolEngine.runOnAxmolThread(new Runnable() {
            @Override
            public void run() {
                if (sNativeInitialized) {
                    /**
                     * onPause may be invoked before onSurfaceCreated,
                     * and engine will be initialized correctly after
                     * onSurfaceCreated is invoked. Can not invoke any
                     * native method before onSurfaceCreated is invoked
                     */
                    AxmolPlayer.nativeOnPause();
                    AxmolEngine.sNativePaused = true;
                }

                mNativePauseComplete.countDown();
            }
        });
    }

    public void queueEvent(Runnable r) {
        mRenderHost.queueEvent(r);
    }

    public void waitForPauseToComplete() {
        while (mNativePauseComplete.getCount() > 0) {
            try {
                mNativePauseComplete.await();
            } catch (InterruptedException e) {
                Log.e(TAG, "Pause wait interrupted", e);
            }
        }
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        super.onLayout(changed, l, t, r, b);
        if (mEnableForceDoLayout) {
            /*This is a hot-fix for some android devices which don't do layout when the main window
             * is paned. We refresh the layout in 24 frames per seconds.
             * When the editBox is lose focus or when user begin to type, the do layout is disabled.
             */
            final Handler handler = new Handler();
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    //Do something after 100ms
                    requestLayout();
                    invalidate();
                }
            }, 1000 / 24);

        }

    }

    // ===========================================================
    // Debug Motion Event (preserve original dumpMotionEvent)
    // ===========================================================

    // private static void dumpMotionEvent(final MotionEvent event) {
    //     final String names[] = { "DOWN", "UP", "MOVE", "CANCEL", "OUTSIDE",
    //         "POINTER_DOWN", "POINTER_UP", "7?", "8?", "9?" };
    //     final StringBuilder sb = new StringBuilder();
    //     final int action = event.getAction();
    //     final int actionCode = action & MotionEvent.ACTION_MASK;
    //     sb.append("event ACTION_").append(names[actionCode]);
    //     if (actionCode == MotionEvent.ACTION_POINTER_DOWN || actionCode == MotionEvent.ACTION_POINTER_UP) {
    //         sb.append("(pid ").append(action >> MotionEvent.ACTION_POINTER_INDEX_SHIFT);
    //         sb.append(")");
    //     }
    //     sb.append("[");
    //     for (int i = 0; i < event.getPointerCount(); i++) {
    //         sb.append("#").append(i);
    //         sb.append("(pid ").append(event.getPointerId(i));
    //         sb.append(")=").append((int) event.getX(i));
    //         sb.append(",").append((int) event.getY(i));
    //         if (i + 1 < event.getPointerCount()) {
    //             sb.append(";");
    //         }
    //     }
    //     sb.append("]");
    //     Log.d(TAG, sb.toString());
    // }

    /**
     * Spawns a platform standard Floating ActionMode window mapped directly to specific coordinates.
     */
    private void showContextMenuOnUIThread(EditMenuParams p) {
        // Prevent overlapping bar windows by closing the older instance first
        if (sCurrentActionMode != null) {
            sCurrentActionMode.finish();
        }

        if (!(mRenderHost instanceof View)) return;
        View targetView = (View) mRenderHost;

        // Use standard Android Callback2 to gain leverage over the absolute positioning rect binding
        android.view.ActionMode.Callback2 callback = new android.view.ActionMode.Callback2() {
            @Override
            public boolean onCreateActionMode(android.view.ActionMode mode, android.view.Menu menu) {
                // Populate custom interaction choices dynamically based on security privileges snapshot
                if (p.hasSelection) {
                    menu.add(0, android.R.id.copy, 0, android.R.string.copy);
                    if (!p.readOnly) {
                        menu.add(0, android.R.id.cut, 1, android.R.string.cut);
                    }
                }
                if (!p.readOnly) {
                    menu.add(0, android.R.id.paste, 2, android.R.string.paste);
                }
                if (p.hasText) {
                    menu.add(0, android.R.id.selectAll, 3, android.R.string.selectAll);
                }
                return true;
            }

            @Override
            public boolean onActionItemClicked(android.view.ActionMode mode, android.view.MenuItem item) {
                int itemId = item.getItemId();
                // Safe tunnel execution back into the synchronized C++ Axmol processing loop
                AxmolEngine.runOnAxmolThread(() -> {
                    if (itemId == android.R.id.copy) {
                        AxmolPlayer.nativePerformEditAction(EDIT_ACTION_COPY);
                    } else if (itemId == android.R.id.cut) {
                        AxmolPlayer.nativePerformEditAction(EDIT_ACTION_CUT);
                    } else if (itemId == android.R.id.paste) {
                        AxmolPlayer.nativePerformEditAction(EDIT_ACTION_PASTE);
                    } else if (itemId == android.R.id.selectAll) {
                        AxmolPlayer.nativePerformEditAction(EDIT_ACTION_SELECT_ALL);
                    }
                });

                mode.finish(); // Dismantle the bar after option is chosen
                return true;
            }

            @Override
            public boolean onPrepareActionMode(android.view.ActionMode mode, android.view.Menu menu) {
                return false;
            }

            @Override
            public void onDestroyActionMode(android.view.ActionMode mode) {
                if (sCurrentActionMode == mode) {
                    sCurrentActionMode = null;
                }
            }

            /**
             * CRUCIAL OVERRIDE: Bound by the OS to fetch the target rect coordinates.
             * This explicitly instructs the floating toolbar bubble exactly where to sit.
             */
            @Override
            public void onGetContentRect(android.view.ActionMode mode, View view, android.graphics.Rect outRect) {
                // Define a tiny virtual bounding bounding block surrounding the exact touch spot.
                // OutRect coordinates must be mapped relative to the targetView local viewport bounds.
                int centerX = (int) p.x;
                int centerY = (int) p.y;
                int targetRadius = 10; // Simple padding buffer radius around the cursor point

                outRect.set(centerX - targetRadius, centerY - targetRadius,
                    centerX + targetRadius, centerY + targetRadius);
            }
        };

        // Initialize the platform native floating context action mode panel (Android 6.0 M and above)
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
            sCurrentActionMode = targetView.startActionMode(callback, android.view.ActionMode.TYPE_FLOATING);
            Log.d("AxmolPlayer", "Triggered system native Floating ActionMode at focus bounds: X=" + p.x + ", Y=" + p.y);
        }
    }


    // ===========================================================
    // Native methods for AxmolPlayer
    // ===========================================================

    public static native void nativeRenderFrame();

    public static native void nativeOnResume();

    public static native void nativeOnPause();

    public static native void nativePerformEditAction(int action);

    // New signature: Sends full layout boundaries and duration when keyboard opens
    public static native void nativeSoftInputShow(float x, float y, float width, float height, float duration);

    // Updated signature: Accepts animation duration when keyboard closes
    public static native void nativeSoftInputHide(float duration);

    public static native void nativeInsertText(String text);

    public static native void nativeDeleteBackward(int numChars);

    public static native void nativeTouchBegin(int id, float x, float y, float pressure);
    public static native void nativeTouchEnd(int id, float x, float y, float pressure);
    public static native void nativeTouchesMove(int[] ids, float[] xs, float[] ys, float[] pressures, int size);
    public static native void nativeTouchesCancel(int[] ids, float[] xs, float[] ys, float[] pressures, int size);

    public static native boolean nativeKeyEvent(final int keyCode, boolean isPressed);

    public static native void nativeOnSurfaceCreated(Object surface, final int width, final int height, boolean isWarmStart);

    public static native void nativeOnSurfaceChanged(final int width, final int height);

    public static native void nativeOnWindowFocusChanged(final boolean hasFocus);
}
