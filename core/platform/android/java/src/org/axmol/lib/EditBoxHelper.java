/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Looper;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.util.SparseArray;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.widget.TextView;


public class EditBoxHelper {
    private static final String TAG = EditBoxHelper.class.getSimpleName();
    private static AxmolActivity mActivity;
    private static ResizeLayout mFrameLayout;

    private static SparseArray<AxmolEditBox> mEditBoxArray;
    private static int mViewTag = 0;
    private static float mPadding = 5.0f;
    //Call native methods
    private static native void editBoxEditingDidBegin(int index);
    public static void __editBoxEditingDidBegin(int index){
        editBoxEditingDidBegin(index);
    }

    private static native void editBoxEditingChanged(int index, String text);
    public static void __editBoxEditingChanged(int index, String text){
        editBoxEditingChanged(index, text);
    }

    private static native void editBoxEditingDidEnd(int index, String text, int action);
    public static void __editBoxEditingDidEnd(int index, String text, int action) {
        editBoxEditingDidEnd(index, text, action);
    }


    public EditBoxHelper(ResizeLayout layout) {
        EditBoxHelper.mFrameLayout = layout;

        EditBoxHelper.mActivity = (AxmolActivity) AxmolActivity.getContext();
        EditBoxHelper.mEditBoxArray = new SparseArray<AxmolEditBox>();
    }

    public static int getPadding(float scaleX){
        return (int)(mPadding*scaleX);
    }

    public static int createEditBox(final int left, final int top, final int width, final int height, final float scaleX) {
        final int index = mViewTag;
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                final AxmolEditBox editBox = new AxmolEditBox(mActivity);
                editBox.setFocusable(true);
                editBox.setFocusableInTouchMode(true);
                editBox.setInputFlag(5); //kEditBoxInputFlagLowercaseAllCharacters
                editBox.setInputMode(6); //kEditBoxInputModeSingleLine
                editBox.setReturnType(0);  //kKeyboardReturnTypeDefault
                editBox.setHintTextColor(Color.GRAY);
                //http://stackoverflow.com/questions/11236336/setvisibilityview-visible-doesnt-always-work-ideas
                editBox.setVisibility(View.GONE);
                editBox.setBackgroundColor(Color.TRANSPARENT);
                editBox.setTextColor(Color.WHITE);
                editBox.setSingleLine();
                editBox.setGLViewScaleX(scaleX);
                editBox.setPadding(getPadding(scaleX), 0, 0, 0);


                FrameLayout.LayoutParams lParams = new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.WRAP_CONTENT,
                        FrameLayout.LayoutParams.WRAP_CONTENT);

                lParams.leftMargin = left;
                lParams.topMargin = top;
                lParams.width = width;
                lParams.height = height;
                lParams.gravity = Gravity.TOP | Gravity.LEFT;

                mFrameLayout.addView(editBox, lParams);
                editBox.setTag(false);
                editBox.addTextChangedListener(new TextWatcher() {
                    @Override
                    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                    }

                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {

                    }

                    //http://stackoverflow.com/questions/21713246/addtextchangedlistener-and-ontextchanged-are-always-called-when-android-fragment

                    @Override
                    public void afterTextChanged(final Editable s) {
                        if (!editBox.getChangedTextProgrammatically()) {
                            if ((Boolean) editBox.getTag()) {
                                AxmolEngine.runOnGLThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        EditBoxHelper.__editBoxEditingChanged(index, s.toString());
                                    }

                                });
                            }
                        }
                        editBox.setChangedTextProgrammatically(false);

                    }
                });


                editBox.setOnFocusChangeListener(new View.OnFocusChangeListener() {

                    @Override
                    public void onFocusChange(View v, boolean hasFocus) {
                        editBox.setTag(true);
                        editBox.setChangedTextProgrammatically(false);
                        if (hasFocus) {
                            AxmolEngine.runOnGLThread(new Runnable() {
                                @Override
                                public void run() {
                                    editBox.endAction = AxmolEditBox.kEndActionUnknown;
                                    EditBoxHelper.__editBoxEditingDidBegin(index);
                                }
                            });
                            editBox.setSelection(editBox.getText().length());
                            mFrameLayout.setEnableForceDoLayout(true);
                            mActivity.getGLSurfaceView().setSoftKeyboardShown(true);
                            Log.d(TAG, "edit box get focus");
                        } else {
                            editBox.setVisibility(View.GONE);
                            // Note that we must to copy a string to prevent string content is modified
                            // on UI thread while 's.toString' is invoked at the same time.
                            final String text = new String(editBox.getText().toString());
                            AxmolEngine.runOnGLThread(new Runnable() {
                                @Override
                                public void run() {
                                    int action = editBox.endAction;
                                    EditBoxHelper.__editBoxEditingDidEnd(index, text, action);
                                }
                            });
                            mActivity.hideVirtualButton();
                            mFrameLayout.setEnableForceDoLayout(false);
                            Log.d(TAG, "edit box lose focus");
                        }
                    }
                });

                editBox.setOnKeyListener(new View.OnKeyListener() {
                    public boolean onKey(View v, int keyCode, KeyEvent event) {
                        // If the event is a key-down event on the "enter" button
                        if ((event.getAction() == KeyEvent.ACTION_DOWN) &&
                                (keyCode == KeyEvent.KEYCODE_ENTER)) {
                            //if editbox doesn't support multiline, just hide the keyboard
                            if ((editBox.getInputType() & InputType.TYPE_TEXT_FLAG_MULTI_LINE) != InputType.TYPE_TEXT_FLAG_MULTI_LINE) {
                                EditBoxHelper.closeKeyboardOnUiThread(index);
                                return true;
                            }
                        }
                        return false;
                    }
                });


                editBox.setOnEditorActionListener(new TextView.OnEditorActionListener() {
                    @Override
                    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                        if (actionId == EditorInfo.IME_ACTION_NEXT) {
                            editBox.endAction = AxmolEditBox.kEndActionNext;
                            EditBoxHelper.closeKeyboardOnUiThread(index);
                            return true;
                        } else if (actionId == EditorInfo.IME_ACTION_DONE || actionId == EditorInfo.IME_ACTION_SEND || actionId == EditorInfo.IME_ACTION_SEARCH || actionId == EditorInfo.IME_ACTION_GO) {
                            editBox.endAction = AxmolEditBox.kEndActionReturn;
                            EditBoxHelper.closeKeyboardOnUiThread(index);
                        }
                        return false;
                    }
                });

                mEditBoxArray.put(index, editBox);
            }
        });
        return mViewTag++;
    }

    public static void removeEditBox(final int index) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    mEditBoxArray.remove(index);
                    mFrameLayout.removeView(editBox);
                    Log.d(TAG, "remove EditBox");
                }
            }
        });
    }

    public static void setFont(final int index, final String fontName, final float fontSize){
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    Typeface tf;
                    if (!fontName.isEmpty()) {
                        if (fontName.endsWith(".ttf")) {
                            try {
                                tf = TypefaceHelper.get(mActivity.getContext(), fontName);
                            } catch (final Exception e) {
                                Log.e("EditBoxHelper", "error to create ttf type face: "
                                        + fontName);
                                // The file may not find, use system font.
                                tf  =  Typeface.create(fontName, Typeface.NORMAL);
                            }
                        } else {
                            tf  =  Typeface.create(fontName, Typeface.NORMAL);
                        }

                    }else{
                        tf = Typeface.DEFAULT;
                    }
                    // TODO: The font size is not the same across all the android devices...
                    if (fontSize >= 0){
                        editBox.setTextSize(TypedValue.COMPLEX_UNIT_PX,
                                fontSize);
                    }
                    editBox.setTypeface(tf);
                }
            }
        });
    }

    public static void setFontColor(final int index, final int red, final int green, final int blue, final int alpha){
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setTextColor(Color.argb(alpha, red, green, blue));
                }
            }
        });
    }

    public static void setPlaceHolderText(final int index, final String text){
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setHint(text);
                }
            }
        });
    }

    public static void setPlaceHolderTextColor(final int index, final int red, final int green, final int blue, final int alpha){
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setHintTextColor(Color.argb(alpha, red, green, blue));
                }
            }
        });
    }

    public static void setMaxLength(final int index, final int maxLength) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setMaxLength(maxLength);
                }
            }
        });
    }

    public static void setVisible(final int index, final boolean visible) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setVisibility(visible ? View.VISIBLE : View.GONE);
                }
            }
        });
    }


    public static void setText(final int index, final String text){
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setChangedTextProgrammatically(true);
                    editBox.setText(text);
                    int position = editBox.getText().length();
                    editBox.setSelection(position);
                }
            }
        });
    }

    public static void setReturnType(final int index, final int returnType) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setReturnType(returnType);
                }
            }
        });
    }

    public static void setTextHorizontalAlignment(final int index, final int alignment) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setTextHorizontalAlignment(alignment);
                }
            }
        });
    }

    public static void setInputMode(final int index, final int inputMode) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setInputMode(inputMode);
                }
            }
        });
    }

    public static void setInputFlag(final int index, final int inputFlag) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setInputFlag(inputFlag);
                }
            }
        });
    }


    public static void setEditBoxViewRect(final int index, final int left, final int top, final int maxWidth, final int maxHeight) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolEditBox editBox = mEditBoxArray.get(index);
                if (editBox != null) {
                    editBox.setEditBoxViewRect(left, top, maxWidth, maxHeight);
                }
            }
        });
    }



    public static void openKeyboard(final int index) {

        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
               openKeyboardOnUiThread(index);
            }
        });
    }

    private static void openKeyboardOnUiThread(int index) {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            Log.e(TAG, "openKeyboardOnUiThread doesn't run on UI thread!");
            return;
        }

        final InputMethodManager imm = (InputMethodManager) mActivity.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        AxmolEditBox editBox = mEditBoxArray.get(index);
        if (null != editBox) {
            editBox.requestFocus();
            mActivity.getGLSurfaceView().requestLayout();
            imm.showSoftInput(editBox, 0);
            mActivity.getGLSurfaceView().setSoftKeyboardShown(true);
        }
    }

    private static void closeKeyboardOnUiThread(int index) {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            Log.e(TAG, "closeKeyboardOnUiThread doesn't run on UI thread!");
            return;
        }

        final InputMethodManager imm = (InputMethodManager) mActivity.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        AxmolEditBox editBox = mEditBoxArray.get(index);
        if (null != editBox) {
            imm.hideSoftInputFromWindow(editBox.getWindowToken(), 0);
            mActivity.getGLSurfaceView().setSoftKeyboardShown(false);
            mActivity.getGLSurfaceView().requestFocus();
            // can take effect after GLSurfaceView has focus
            mActivity.hideVirtualButton();
        }
    }

    // Note that closeKeyboard will be invoked on GL thread
    public static void closeKeyboard(final int index) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                closeKeyboardOnUiThread(index);
            }
        });
    }
}
