/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

public class TextInputWrapper implements TextWatcher, OnEditorActionListener {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final String TAG = TextInputWrapper.class.getSimpleName();

    // ===========================================================
    // Fields
    // ===========================================================

    private final AxmolGLSurfaceView mGLSurfaceView;
    private String mText = "";
    private String mOriginText;
    private boolean mRangeInput;
    private boolean mActive;
    private boolean mSelectionPosted;

    // ===========================================================
    // Constructors
    // ===========================================================

    public TextInputWrapper(final AxmolGLSurfaceView surfaceView) {
        mGLSurfaceView = surfaceView;
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    private boolean isFullScreenEdit() {
        final TextView field = mGLSurfaceView.getEditText();
        final InputMethodManager imm = (InputMethodManager)
            field.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        return imm != null && imm.isFullscreenMode();
    }

    public void setOriginText(final String text) {
        mOriginText = text;
        mText = text;
    }

    public void beginInput(final String text, final boolean rangeInput) {
        setOriginText(text);
        mRangeInput = rangeInput;
        mActive = true;
    }

    public void endInput() {
        mActive = false;
    }

    public boolean isRangeInputActive() {
        return mActive && mRangeInput;
    }

    public void scheduleSelectionUpdate() {
        if (!isRangeInputActive() || mSelectionPosted) {
            return;
        }
        mSelectionPosted = true;
        mGLSurfaceView.getEditText().post(new Runnable() {
            @Override
            public void run() {
                mSelectionPosted = false;
                if (isRangeInputActive()) {
                    final AxmolEditBox field = mGLSurfaceView.getEditText();
                    mGLSurfaceView.updateTextSelection(
                        field.getSelectionStart(), field.getSelectionEnd());
                }
            }
        });
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    @Override
    public void beforeTextChanged(final CharSequence text, final int start, final int count, final int after) {
        mText = text.toString();
    }

    @Override
    public void onTextChanged(final CharSequence text, final int start, final int before, final int count) {
        if (!mActive) {
            return;
        }

        if (mRangeInput) {
            mGLSurfaceView.replaceTextRange(
                start, start + before,
                text.subSequence(start, start + count).toString(), text.toString());
            return;
        }

        // Compatibility path for delegates without the new range interface.
        if (isFullScreenEdit()) {
            return;
        }

        if (before > 0) {
            mGLSurfaceView.deleteBackward(mText.codePointCount(start, start + before));
        }

        if (count > 0) {
            mGLSurfaceView.insertText(text.subSequence(start, start + count).toString());
        }
    }

    @Override
    public void afterTextChanged(final Editable text) {
        mText = text.toString();
        scheduleSelectionUpdate();
    }

    @Override
    public boolean onEditorAction(final TextView pTextView, final int pActionID, final KeyEvent pKeyEvent) {
        if (mGLSurfaceView.getEditText() != pTextView) {
            return false;
        }
        if (isRangeInputActive()) {
            if (pActionID == EditorInfo.IME_ACTION_DONE ||
                (pActionID == EditorInfo.IME_NULL && pKeyEvent != null &&
                 pKeyEvent.getKeyCode() == KeyEvent.KEYCODE_ENTER &&
                 pKeyEvent.getAction() == KeyEvent.ACTION_DOWN)) {
                mGLSurfaceView.finishTextInput();
                return true;
            }
            return false;
        }

        if (isFullScreenEdit()) {
            if (mOriginText != null && !mOriginText.isEmpty()) {
                mGLSurfaceView.deleteBackward(
                    mOriginText.codePointCount(0, mOriginText.length()));
            }

            String text = pTextView.getText().toString();
            if (!text.endsWith("\n")) {
                text += "\n";
            }
            mGLSurfaceView.insertText(text);
        }

        if (pActionID == EditorInfo.IME_ACTION_DONE) {
            mGLSurfaceView.requestFocus();
        }
        return false;
    }
}
