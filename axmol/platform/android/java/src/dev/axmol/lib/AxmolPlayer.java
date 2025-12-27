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

import java.lang.ref.WeakReference;
import java.util.concurrent.CountDownLatch;

public class AxmolPlayer extends FrameLayout {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final String TAG = AxmolPlayer.class.getSimpleName();

    private final static int HANDLER_OPEN_IME_KEYBOARD = 2;
    private final static int HANDLER_CLOSE_IME_KEYBOARD = 3;

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

    private AxmolEditBox mEditBox;
    private TextInputListener mTextInputListener;

    private AxmolRenderHost mRenderHost; // GLSurfaceView or SurfaceView

    private boolean mSoftKeyboardShown = false;
    private boolean mMultipleTouchEnabled = true;

    private boolean mEnableForceDoLayout = false;

    private boolean mIsVulkan = false;

    private boolean mSurfaceCreated = false;

    private CountDownLatch mNativePauseComplete;

    private WebViewHelper mWebViewHelper = null;
    private EditBoxHelper mEditBoxHelper = null;

    private int mLastSurfaceWidth = 0;
    private int mLastSurfaceHeight = 0;

    public boolean isSoftKeyboardShown() {
        return mSoftKeyboardShown;
    }

    public void setSoftKeyboardShown(boolean softKeyboardShown) {
        this.mSoftKeyboardShown = softKeyboardShown;
    }

    public boolean isMultipleTouchEnabled() {
        return mMultipleTouchEnabled;
    }

    public void setEnableForceDoLayout(boolean flag) {
        mEnableForceDoLayout = flag;
    }

    public AxmolEditBox getEditText() {
        return this.mEditBox;
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

        mIsVulkan = AxmolEngine.nativeGetRenderAPI() == AxmolEngine.RENDER_API_VK;

        // FrameLayout
        LayoutParams frameParams = new LayoutParams(
            LayoutParams.MATCH_PARENT,
            LayoutParams.MATCH_PARENT
        );
        setLayoutParams(frameParams);

        // Create hidden EditBox
        mEditBox = new AxmolEditBox(ctx);
        LayoutParams editParams = new LayoutParams(
            LayoutParams.MATCH_PARENT,
            LayoutParams.WRAP_CONTENT
        );
        mEditBox.setLayoutParams(editParams);
        mEditBox.setVisibility(View.GONE);
        mEditBox.setImeOptions(EditorInfo.IME_ACTION_DONE);
        addView(mEditBox);

        // Text input wrapper
        mTextInputListener = new TextInputListener(this);
        mEditBox.setOnEditorActionListener(mTextInputListener);

        // Handler for IME open/close
        sHandler = new Handler(msg -> {
            switch (msg.what) {
                case HANDLER_OPEN_IME_KEYBOARD:
                    if (mEditBox != null) {
                        mEditBox.setVisibility(View.VISIBLE);
                        if (mEditBox.requestFocus()) {
                            mEditBox.removeTextChangedListener(mTextInputListener);
                            mEditBox.setText("");
                            final String text = (String) msg.obj;
                            mEditBox.append(text);
                            mTextInputListener.setOriginText(text);
                            mEditBox.addTextChangedListener(mTextInputListener);
                            InputMethodManager imm = (InputMethodManager) ctx.getSystemService(Context.INPUT_METHOD_SERVICE);
                            imm.showSoftInput(mEditBox, 0);
                            Log.d(TAG, "showSoftInput");
                        }
                    }
                    break;
                case HANDLER_CLOSE_IME_KEYBOARD:
                    if (mEditBox != null) {
                        mEditBox.removeTextChangedListener(mTextInputListener);
                        InputMethodManager imm = (InputMethodManager) ctx.getSystemService(Context.INPUT_METHOD_SERVICE);
                        imm.hideSoftInputFromWindow(mEditBox.getWindowToken(), 0);
                        requestFocus();
                        mEditBox.setVisibility(View.GONE);
                        if (ctx instanceof AxmolActivity) {
                            ((AxmolActivity) ctx).hideVirtualButton();
                        }
                        Log.d(TAG, "HideSoftInput");
                    }
                    break;
            }
            return true;
        });

        // Decide backend
        if (mIsVulkan) {
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
    }

    private void initVulkanView(Context ctx) {
        AxmolSurfaceViewVK surfaceView = new AxmolSurfaceViewVK(this);
        mRenderHost = surfaceView;
        addView(surfaceView);
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
        AxmolPlayer player = getInstance();
        if (player == null) return;
        final Message msg = new Message();
        msg.what = HANDLER_OPEN_IME_KEYBOARD;
        msg.obj = player.getContentText();
        sHandler.sendMessage(msg);
    }

    @SuppressWarnings("unused")
    public static void closeIMEKeyboard() {
        final Message msg = new Message();
        msg.what = HANDLER_CLOSE_IME_KEYBOARD;
        sHandler.sendMessage(msg);
    }

    public void insertText(final String text) {
        AxmolPlayer.nativeInsertText(text);
    }

    public void deleteBackward(int numChars) {
        AxmolPlayer.nativeDeleteBackward(numChars);
    }

    private String getContentText() {
        return AxmolPlayer.nativeGetContentText();
    }

    // ===========================================================
    // Touch & Key Events
    // ===========================================================

    @Override
    public boolean onTouchEvent(final MotionEvent pMotionEvent) {
        // these data are used in ACTION_MOVE and ACTION_CANCEL
        final int pointerNumber = pMotionEvent.getPointerCount();
        final int[] ids = new int[pointerNumber];
        final float[] xs = new float[pointerNumber];
        final float[] ys = new float[pointerNumber];

        if (mSoftKeyboardShown) {
            InputMethodManager imm = (InputMethodManager) this.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            View view = ((Activity) this.getContext()).getCurrentFocus();
            if (null != view) {
                imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
            }
            this.requestFocus();
            mSoftKeyboardShown = false;
        }

        for (int i = 0; i < pointerNumber; i++) {
            ids[i] = pMotionEvent.getPointerId(i);
            xs[i] = pMotionEvent.getX(i);
            ys[i] = pMotionEvent.getY(i);
        }

        switch (pMotionEvent.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_POINTER_DOWN:
                final int indexPointerDown = pMotionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                if (!mMultipleTouchEnabled && indexPointerDown != 0) {
                    break;
                }
                final int idPointerDown = pMotionEvent.getPointerId(indexPointerDown);
                final float xPointerDown = pMotionEvent.getX(indexPointerDown);
                final float yPointerDown = pMotionEvent.getY(indexPointerDown);

                AxmolEngine.runOnAxmolThread(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeTouchesBegin(idPointerDown, xPointerDown, yPointerDown);
                    }
                });
                break;

            case MotionEvent.ACTION_DOWN:
                // there are only one finger on the screen
                final int idDown = pMotionEvent.getPointerId(0);
                final float xDown = xs[0];
                final float yDown = ys[0];

                AxmolEngine.runOnAxmolThread(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeTouchesBegin(idDown, xDown, yDown);
                    }
                });
                break;

            case MotionEvent.ACTION_MOVE:
                if (!mMultipleTouchEnabled) {
                    // handle only touch with id == 0
                    for (int i = 0; i < pointerNumber; i++) {
                        if (ids[i] == 0) {
                            final int[] idsMove = new int[]{0};
                            final float[] xsMove = new float[]{xs[i]};
                            final float[] ysMove = new float[]{ys[i]};
                            AxmolEngine.runOnAxmolThread(new Runnable() {
                                @Override
                                public void run() {
                                    AxmolPlayer.nativeTouchesMove(idsMove, xsMove, ysMove);
                                }
                            });
                            break;
                        }
                    }
                } else {
                    AxmolEngine.runOnAxmolThread(new Runnable() {
                        @Override
                        public void run() {
                            AxmolPlayer.nativeTouchesMove(ids, xs, ys);
                        }
                    });
                }
                break;

            case MotionEvent.ACTION_POINTER_UP:
                final int indexPointUp = pMotionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                if (!mMultipleTouchEnabled && indexPointUp != 0) {
                    break;
                }
                final int idPointerUp = pMotionEvent.getPointerId(indexPointUp);
                final float xPointerUp = pMotionEvent.getX(indexPointUp);
                final float yPointerUp = pMotionEvent.getY(indexPointUp);

                AxmolEngine.runOnAxmolThread(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeTouchesEnd(idPointerUp, xPointerUp, yPointerUp);
                    }
                });
                break;

            case MotionEvent.ACTION_UP:
                // there are only one finger on the screen
                final int idUp = pMotionEvent.getPointerId(0);
                final float xUp = xs[0];
                final float yUp = ys[0];

                AxmolEngine.runOnAxmolThread(new Runnable() {
                    @Override
                    public void run() {
                        AxmolPlayer.nativeTouchesEnd(idUp, xUp, yUp);
                    }
                });
                break;

            case MotionEvent.ACTION_CANCEL:
                if (!mMultipleTouchEnabled) {
                    // handle only touch with id == 0
                    for (int i = 0; i < pointerNumber; i++) {
                        if (ids[i] == 0) {
                            final int[] idsCancel = new int[]{0};
                            final float[] xsCancel = new float[]{xs[i]};
                            final float[] ysCancel = new float[]{ys[i]};
                            AxmolEngine.runOnAxmolThread(new Runnable() {
                                @Override
                                public void run() {
                                    AxmolPlayer.nativeTouchesCancel(idsCancel, xsCancel, ysCancel);
                                }
                            });
                            break;
                        }
                    }
                } else {
                    AxmolEngine.runOnAxmolThread(new Runnable() {
                        @Override
                        public void run() {
                            AxmolPlayer.nativeTouchesCancel(ids, xs, ys);
                        }
                    });
                }
                break;
        }

        /*
        if (BuildConfig.DEBUG) {
            AxmolSurfaceViewGL.dumpMotionEvent(pMotionEvent);
        }
        */
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


    // ===========================================================
    // Native methods for AxmolPlayer
    // ===========================================================

    public static native void nativeRenderFrame();

    public static native void nativeOnResume();

    public static native void nativeOnPause();

    public static native String nativeGetContentText();

    public static native void nativeInsertText(String text);

    public static native void nativeDeleteBackward(int numChars);

    public static native void nativeTouchesBegin(final int id, final float x, final float y);

    public static native void nativeTouchesEnd(final int id, final float x, final float y);

    public static native void nativeTouchesMove(final int[] ids, final float[] xs, final float[] ys);

    public static native void nativeTouchesCancel(final int[] ids, final float[] xs, final float[] ys);

    public static native boolean nativeKeyEvent(final int keyCode, boolean isPressed);

    public static native void nativeOnSurfaceCreated(Object surface, final int width, final int height, boolean isWarmStart);

    public static native void nativeOnSurfaceChanged(final int width, final int height);
}
