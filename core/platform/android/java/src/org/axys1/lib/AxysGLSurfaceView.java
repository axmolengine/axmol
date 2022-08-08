/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

https://axys1.github.io/

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
package org.axys1.lib;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

public class AxysGLSurfaceView extends GLSurfaceView {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final String TAG = AxysGLSurfaceView.class.getSimpleName();

    private final static int HANDLER_OPEN_IME_KEYBOARD = 2;
    private final static int HANDLER_CLOSE_IME_KEYBOARD = 3;

    // ===========================================================
    // Fields
    // ===========================================================

    // TODO Static handler -> Potential leak!
    private static Handler sHandler;

    private static AxysGLSurfaceView mAxysGLSurfaceView;
    private static AxysTextInputWrapper sAxysTextInputWraper;

    private AxysRenderer mAxysRenderer;
    private AxysEditBox mAxysEditText;

    private boolean mSoftKeyboardShown = false;
    private boolean mMultipleTouchEnabled = true;

    public boolean isSoftKeyboardShown() {
        return mSoftKeyboardShown;
    }

    public void setSoftKeyboardShown(boolean softKeyboardShown) {
        this.mSoftKeyboardShown = softKeyboardShown;
    }

    public boolean isMultipleTouchEnabled() {
        return mMultipleTouchEnabled;
    }

    public void setMultipleTouchEnabled(boolean multipleTouchEnabled) {
        this.mMultipleTouchEnabled = multipleTouchEnabled;
    }

    // ===========================================================
    // Constructors
    // ===========================================================

    public AxysGLSurfaceView(final Context context) {
        super(context);

        this.initView();
    }

    public AxysGLSurfaceView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        
        this.initView();
    }

    protected void initView() {
        this.setEGLContextClientVersion(2);
        this.setFocusableInTouchMode(true);

        AxysGLSurfaceView.mAxysGLSurfaceView = this;
        AxysGLSurfaceView.sAxysTextInputWraper = new AxysTextInputWrapper(this);

        AxysGLSurfaceView.sHandler = new Handler() {
            @Override
            public void handleMessage(final Message msg) {
                switch (msg.what) {
                    case HANDLER_OPEN_IME_KEYBOARD:
                        if (null != AxysGLSurfaceView.this.mAxysEditText && AxysGLSurfaceView.this.mAxysEditText.requestFocus()) {
                            AxysGLSurfaceView.this.mAxysEditText.removeTextChangedListener(AxysGLSurfaceView.sAxysTextInputWraper);
                            AxysGLSurfaceView.this.mAxysEditText.setText("");
                            final String text = (String) msg.obj;
                            AxysGLSurfaceView.this.mAxysEditText.append(text);
                            AxysGLSurfaceView.sAxysTextInputWraper.setOriginText(text);
                            AxysGLSurfaceView.this.mAxysEditText.addTextChangedListener(AxysGLSurfaceView.sAxysTextInputWraper);
                            final InputMethodManager imm = (InputMethodManager) AxysGLSurfaceView.mAxysGLSurfaceView.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                            imm.showSoftInput(AxysGLSurfaceView.this.mAxysEditText, 0);
                            Log.d("GLSurfaceView", "showSoftInput");
                        }
                        break;

                    case HANDLER_CLOSE_IME_KEYBOARD:
                        if (null != AxysGLSurfaceView.this.mAxysEditText) {
                            AxysGLSurfaceView.this.mAxysEditText.removeTextChangedListener(AxysGLSurfaceView.sAxysTextInputWraper);
                            final InputMethodManager imm = (InputMethodManager) AxysGLSurfaceView.mAxysGLSurfaceView.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                            imm.hideSoftInputFromWindow(AxysGLSurfaceView.this.mAxysEditText.getWindowToken(), 0);
                            AxysGLSurfaceView.this.requestFocus();
                            // can take effect after GLSurfaceView has focus
                            ((AxysActivity)AxysGLSurfaceView.mAxysGLSurfaceView.getContext()).hideVirtualButton();
                            Log.d("GLSurfaceView", "HideSoftInput");
                        }
                        break;
                }
            }
        };
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================


       public static AxysGLSurfaceView getInstance() {
       return mAxysGLSurfaceView;
       }

       public static void queueAccelerometer(final float x, final float y, final float z, final long timestamp) {   
       mAxysGLSurfaceView.queueEvent(new Runnable() {
        @Override
            public void run() {
                AxysAccelerometer.onSensorChanged(x, y, z, timestamp);
        }
        });
    }

    public void setAxysRenderer(final AxysRenderer renderer) {
        this.mAxysRenderer = renderer;
        this.setRenderer(this.mAxysRenderer);
    }

    private String getContentText() {
        return this.mAxysRenderer.getContentText();
    }

    public AxysEditBox getAxysEditText() {
        return this.mAxysEditText;
    }

    public void setAxysEditText(final AxysEditBox pAxysEditText) {
        this.mAxysEditText = pAxysEditText;
        if (null != this.mAxysEditText && null != AxysGLSurfaceView.sAxysTextInputWraper) {
            this.mAxysEditText.setOnEditorActionListener(AxysGLSurfaceView.sAxysTextInputWraper);
            this.requestFocus();
        }
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    @Override
    public void onResume() {
        super.onResume();
        this.setRenderMode(RENDERMODE_CONTINUOUSLY);
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                AxysGLSurfaceView.this.mAxysRenderer.handleOnResume();
            }
        });
    }

    @Override
    public void onPause() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                AxysGLSurfaceView.this.mAxysRenderer.handleOnPause();
            }
        });
        this.setRenderMode(RENDERMODE_WHEN_DIRTY);
        super.onPause();
    }

    @Override
    public boolean onTouchEvent(final MotionEvent pMotionEvent) {
        // these data are used in ACTION_MOVE and ACTION_CANCEL
        final int pointerNumber = pMotionEvent.getPointerCount();
        final int[] ids = new int[pointerNumber];
        final float[] xs = new float[pointerNumber];
        final float[] ys = new float[pointerNumber];

        if (mSoftKeyboardShown){
            InputMethodManager imm = (InputMethodManager)this.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            View view = ((Activity)this.getContext()).getCurrentFocus();
            imm.hideSoftInputFromWindow(view.getWindowToken(),0);
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

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxysGLSurfaceView.this.mAxysRenderer.handleActionDown(idPointerDown, xPointerDown, yPointerDown);
                    }
                });
                break;

            case MotionEvent.ACTION_DOWN:
                // there are only one finger on the screen
                final int idDown = pMotionEvent.getPointerId(0);
                final float xDown = xs[0];
                final float yDown = ys[0];

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxysGLSurfaceView.this.mAxysRenderer.handleActionDown(idDown, xDown, yDown);
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
                            this.queueEvent(new Runnable() {
                                @Override
                                public void run() {
                                    AxysGLSurfaceView.this.mAxysRenderer.handleActionMove(idsMove, xsMove, ysMove);
                                }
                            });
                            break;
                        }
                    }
                } else {
                    this.queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            AxysGLSurfaceView.this.mAxysRenderer.handleActionMove(ids, xs, ys);
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

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxysGLSurfaceView.this.mAxysRenderer.handleActionUp(idPointerUp, xPointerUp, yPointerUp);
                    }
                });
                break;

            case MotionEvent.ACTION_UP:
                // there are only one finger on the screen
                final int idUp = pMotionEvent.getPointerId(0);
                final float xUp = xs[0];
                final float yUp = ys[0];

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxysGLSurfaceView.this.mAxysRenderer.handleActionUp(idUp, xUp, yUp);
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
                            this.queueEvent(new Runnable() {
                                @Override
                                public void run() {
                                    AxysGLSurfaceView.this.mAxysRenderer.handleActionCancel(idsCancel, xsCancel, ysCancel);
                                }
                            });
                            break;
                        }
                    }
                } else {
                    this.queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            AxysGLSurfaceView.this.mAxysRenderer.handleActionCancel(ids, xs, ys);
                        }
                    });
                }
                break;
        }

        /*
        if (BuildConfig.DEBUG) {
            AxysGLSurfaceView.dumpMotionEvent(pMotionEvent);
        }
        */
        return true;
    }

    /*
     * This function is called before AxysRenderer.nativeInit(), so the
     * width and height is correct.
     */
    @Override
    protected void onSizeChanged(final int pNewSurfaceWidth, final int pNewSurfaceHeight, final int pOldSurfaceWidth, final int pOldSurfaceHeight) {
        if(!this.isInEditMode()) {
            this.mAxysRenderer.setScreenWidthAndHeight(pNewSurfaceWidth, pNewSurfaceHeight);
        }
    }

    @Override
    public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
        switch (pKeyCode) {
            case KeyEvent.KEYCODE_BACK:
                AxysVideoHelper.mVideoHandler.sendEmptyMessage(AxysVideoHelper.KeyEventBack);
            case KeyEvent.KEYCODE_MENU:
            case KeyEvent.KEYCODE_DPAD_LEFT:
            case KeyEvent.KEYCODE_DPAD_RIGHT:
            case KeyEvent.KEYCODE_DPAD_UP:
            case KeyEvent.KEYCODE_DPAD_DOWN:
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE:
            case KeyEvent.KEYCODE_DPAD_CENTER:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxysGLSurfaceView.this.mAxysRenderer.handleKeyDown(pKeyCode);
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
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        AxysGLSurfaceView.this.mAxysRenderer.handleKeyUp(keyCode);
                    }
                });
                return true;
            default:
                return super.onKeyUp(keyCode, event);
        }
    }

    // ===========================================================
    // Methods
    // ===========================================================

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public static void openIMEKeyboard() {
        final Message msg = new Message();
        msg.what = AxysGLSurfaceView.HANDLER_OPEN_IME_KEYBOARD;
        msg.obj = AxysGLSurfaceView.mAxysGLSurfaceView.getContentText();
        AxysGLSurfaceView.sHandler.sendMessage(msg);
    }

    public static void closeIMEKeyboard() {
        final Message msg = new Message();
        msg.what = AxysGLSurfaceView.HANDLER_CLOSE_IME_KEYBOARD;
        AxysGLSurfaceView.sHandler.sendMessage(msg);
    }

    public void insertText(final String pText) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                AxysGLSurfaceView.this.mAxysRenderer.handleInsertText(pText);
            }
        });
    }

    public void deleteBackward() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                AxysGLSurfaceView.this.mAxysRenderer.handleDeleteBackward();
            }
        });
    }

    private static void dumpMotionEvent(final MotionEvent event) {
        final String names[] = { "DOWN", "UP", "MOVE", "CANCEL", "OUTSIDE", "POINTER_DOWN", "POINTER_UP", "7?", "8?", "9?" };
        final StringBuilder sb = new StringBuilder();
        final int action = event.getAction();
        final int actionCode = action & MotionEvent.ACTION_MASK;
        sb.append("event ACTION_").append(names[actionCode]);
        if (actionCode == MotionEvent.ACTION_POINTER_DOWN || actionCode == MotionEvent.ACTION_POINTER_UP) {
            sb.append("(pid ").append(action >> MotionEvent.ACTION_POINTER_INDEX_SHIFT);
            sb.append(")");
        }
        sb.append("[");
        for (int i = 0; i < event.getPointerCount(); i++) {
            sb.append("#").append(i);
            sb.append("(pid ").append(event.getPointerId(i));
            sb.append(")=").append((int) event.getX(i));
            sb.append(",").append((int) event.getY(i));
            if (i + 1 < event.getPointerCount()) {
                sb.append(";");
            }
        }
        sb.append("]");
        Log.d(AxysGLSurfaceView.TAG, sb.toString());
    }
}
