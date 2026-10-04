/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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
import android.graphics.Typeface;
import android.text.Editable;
import android.text.InputFilter;
import android.text.InputType;
import android.text.Spanned;
import android.text.SpannableString;
import android.text.TextUtils;
import java.util.ArrayList;
import android.text.method.PasswordTransformationMethod;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.widget.FrameLayout;

import androidx.appcompat.widget.AppCompatEditText;

public class AxmolEditBox extends AppCompatEditText {
    /**
     * The user is allowed to enter any text, including line breaks.
     */
    private final int kEditBoxInputModeAny = 0;

    /**
     * The user is allowed to enter an e-mail address.
     */
    private final int kEditBoxInputModeEmailAddr = 1;

    /**
     * The user is allowed to enter an integer value.
     */
    private final int kEditBoxInputModeNumeric = 2;

    /**
     * The user is allowed to enter a phone number.
     */
    private final int kEditBoxInputModePhoneNumber = 3;

    /**
     * The user is allowed to enter a URL.
     */
    private final int kEditBoxInputModeUrl = 4;

    /**
     * The user is allowed to enter a real number value. This extends kEditBoxInputModeNumeric by allowing a decimal point.
     */
    private final int kEditBoxInputModeDecimal = 5;

    /**
     * The user is allowed to enter any text, except for line breaks.
     */
    private final int kEditBoxInputModeSingleLine = 6;

    /**
     * Indicates that the text entered is confidential data that should be obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
     */
    private final int kEditBoxInputFlagPassword = 0;

    /**
     * Indicates that the text entered is sensitive data that the implementation must never store into a dictionary or table for use in predictive, auto-completing, or other accelerated input schemes. A credit card number is an example of sensitive data.
     */
    private final int kEditBoxInputFlagSensitive = 1;

    /**
     * This flag is a hint to the implementation that during text editing, the initial letter of each word should be capitalized.
     */
    private final int kEditBoxInputFlagInitialCapsWord = 2;

    /**
     * This flag is a hint to the implementation that during text editing, the initial letter of each sentence should be capitalized.
     */
    private final int kEditBoxInputFlagInitialCapsSentence = 3;

    /**
     * Capitalize all characters automatically.
     */
    private final int kEditBoxInputFlagInitialCapsAllCharacters = 4;

    /**
     *  Lowercase all characters automatically.
     */
    private final int kEditBoxInputFlagLowercaseAllCharacters = 5;

    private final int kKeyboardReturnTypeDefault = 0;
    private final int kKeyboardReturnTypeDone = 1;
    private final int kKeyboardReturnTypeSend = 2;
    private final int kKeyboardReturnTypeSearch = 3;
    private final int kKeyboardReturnTypeGo = 4;
    private final int kKeyboardReturnTypeNext = 5;

    public static final int kEndActionUnknown = 0;
    public static final int kEndActionNext = 1;
    public static final int kEndActionReturn = 3;

    private static final int kTextHorizontalAlignmentLeft = 0;
    private static final int kTextHorizontalAlignmentCenter = 1;
    private static final int kTextHorizontalAlignmentRight = 2;

    private static final int kTextVerticalAlignmentTop = 0;
    private static final int kTextVerticalAlignmentCenter = 1;
    private static final int kTextVerticalAlignmentBottom = 2;

    private int mInputFlagConstraints;
    private int mInputModeConstraints;
    private InputFilter mTextFieldLengthFilter;

    // This limit applies to the shared input view used by TextFieldEx.
    // Zero means unlimited. Preserve unrelated input filters.
    public void setTextFieldMaxLength(final int maxLength) {
        final ArrayList<InputFilter> filters = new ArrayList<InputFilter>();

        for (final InputFilter filter : getFilters()) {
            if (filter != mTextFieldLengthFilter &&
                !(filter instanceof InputFilter.LengthFilter)) {
                filters.add(filter);
            }
        }

        mTextFieldLengthFilter = null;

        if (maxLength > 0) {
            mTextFieldLengthFilter = new CodePointLengthFilter(maxLength);
            filters.add(mTextFieldLengthFilter);
        }

        setFilters(filters.toArray(new InputFilter[0]));
    }

    private static final class CodePointLengthFilter implements InputFilter {
        private final int mMaxLength;

        CodePointLengthFilter(final int maxLength) {
            mMaxLength = maxLength;
        }

        @Override
        public CharSequence filter(
            final CharSequence source, final int start, final int end,
            final Spanned dest, final int dstart, final int dend) {
            // A deletion must be permitted even if the buffer exceeds the limit.
            if (start == end) {
                return null;
            }

            final int existingCount = Character.codePointCount(dest, 0, dest.length());
            final int replacedCount = Character.codePointCount(dest, dstart, dend);
            final int available = mMaxLength - (existingCount - replacedCount);

            if (available <= 0) {
                return "";
            }

            if (Character.codePointCount(source, start, end) <= available) {
                return null;
            }

            int acceptedEnd = start;

            for (int count = 0; count < available; ++count) {
                final char current = source.charAt(acceptedEnd);
                ++acceptedEnd;

                if (Character.isHighSurrogate(current) && acceptedEnd < end &&
                    Character.isLowSurrogate(source.charAt(acceptedEnd))) {
                    ++acceptedEnd;
                }
            }

            final CharSequence accepted = source.subSequence(start, acceptedEnd);

            if (source instanceof Spanned) {
                final SpannableString result = new SpannableString(accepted);
                TextUtils.copySpansFrom((Spanned)source, start, acceptedEnd, null, result, 0);
                return result;
            }

            return accepted;
        }
    }

    private int mMaxLength;

    public Boolean getChangedTextProgrammatically() {
        return changedTextProgrammatically;
    }

    public void setChangedTextProgrammatically(Boolean changedTextProgrammatically) {
        this.changedTextProgrammatically = changedTextProgrammatically;
    }

    private Boolean changedTextProgrammatically = false;

    //OpenGL view scaleX
    private  float mScaleX;

    // package private
    int endAction = kEndActionUnknown;


    public  AxmolEditBox(Context context){
        super(context);
        this.setTextHorizontalAlignment(kTextHorizontalAlignmentLeft);
        this.setTextVerticalAlignment(kTextVerticalAlignmentCenter);
    }

    public void setEditBoxViewRect(int left, int top, int maxWidth, int maxHeight) {
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
                                                                            FrameLayout.LayoutParams.WRAP_CONTENT);
        layoutParams.leftMargin = left;
        layoutParams.topMargin = top;
        layoutParams.width = maxWidth;
        layoutParams.height = maxHeight;
        layoutParams.gravity = Gravity.TOP | Gravity.LEFT;
        this.setLayoutParams(layoutParams);
    }

    public float getRenderViewScaleX() {
        return mScaleX;
    }

    public void setRenderViewScaleX(float mScaleX) {
        this.mScaleX = mScaleX;
    }


    public  void setMaxLength(int maxLength){
        this.mMaxLength = maxLength;

        this.setFilters(new InputFilter[]{new InputFilter.LengthFilter(this.mMaxLength) });
    }

    public void setMultilineEnabled(boolean flag){
        this.mInputModeConstraints |= InputType.TYPE_TEXT_FLAG_MULTI_LINE;
    }

    public void setReturnType(int returnType) {
        switch (returnType) {
            case kKeyboardReturnTypeDefault:
                this.setImeOptions(EditorInfo.IME_ACTION_NONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
            case kKeyboardReturnTypeDone:
                this.setImeOptions(EditorInfo.IME_ACTION_DONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
            case kKeyboardReturnTypeSend:
                this.setImeOptions(EditorInfo.IME_ACTION_SEND | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
            case kKeyboardReturnTypeSearch:
                this.setImeOptions(EditorInfo.IME_ACTION_SEARCH | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
            case kKeyboardReturnTypeGo:
                this.setImeOptions(EditorInfo.IME_ACTION_GO | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
            case kKeyboardReturnTypeNext:
                this.setImeOptions(EditorInfo.IME_ACTION_NEXT | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
            default:
                this.setImeOptions(EditorInfo.IME_ACTION_NONE | EditorInfo.IME_FLAG_NO_EXTRACT_UI);
                break;
        }
    }

    public void setTextHorizontalAlignment(int alignment) {
        int gravity = this.getGravity();
        gravity &= ~Gravity.RELATIVE_HORIZONTAL_GRAVITY_MASK;
        switch (alignment) {
            case kTextHorizontalAlignmentLeft:
                gravity |= Gravity.LEFT;
                break;
            case kTextHorizontalAlignmentCenter:
                gravity |= Gravity.CENTER_HORIZONTAL;
                break;
            case kTextHorizontalAlignmentRight:
                gravity |= Gravity.RIGHT;
                break;
            default:
                gravity |= Gravity.LEFT;
                break;
        }
        this.setGravity(gravity);
    }

    public void setTextVerticalAlignment(int alignment) {
        int gravity = this.getGravity();
        int padding = EditBoxHelper.getPadding(mScaleX);
        switch (alignment) {
            case kTextVerticalAlignmentTop:
                setPadding(padding, padding*3/4, 0, 0);
                gravity = (gravity & ~Gravity.BOTTOM) | Gravity.TOP ;
                break;
            case kTextVerticalAlignmentCenter:
                setPadding(padding, 0, 0, padding/2);
                gravity =(gravity & ~Gravity.TOP & ~Gravity.BOTTOM) | Gravity.CENTER_VERTICAL;
                break;
            case kTextVerticalAlignmentBottom:
                //TODO: Add appropriate padding when this alignment is used
                gravity = (gravity & ~Gravity.TOP) | Gravity.BOTTOM ;
                break;
            default:
                setPadding(padding, 0, 0, padding/2);
                gravity =(gravity & ~Gravity.TOP & ~Gravity.BOTTOM) | Gravity.CENTER_VERTICAL;
                break;
        }

        this.setGravity(gravity);
    }

    public  void setInputMode(int inputMode){
        switch (inputMode) {
            case kEditBoxInputModeAny:
                this.setTextVerticalAlignment(kTextVerticalAlignmentTop);
                this.mInputModeConstraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_MULTI_LINE;
                break;
            case kEditBoxInputModeEmailAddr:
                this.mInputModeConstraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
                break;
            case kEditBoxInputModeNumeric:
                this.mInputModeConstraints = InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_SIGNED;
                break;
            case kEditBoxInputModePhoneNumber:
                this.mInputModeConstraints = InputType.TYPE_CLASS_PHONE;
                break;
            case kEditBoxInputModeUrl:
                this.mInputModeConstraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI;
                break;
            case kEditBoxInputModeDecimal:
                this.mInputModeConstraints = InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL | InputType.TYPE_NUMBER_FLAG_SIGNED;
                break;
            case kEditBoxInputModeSingleLine:
                this.mInputModeConstraints = InputType.TYPE_CLASS_TEXT;
                break;
            default:

                break;
        }

        this.setInputType(this.mInputModeConstraints | this.mInputFlagConstraints);
    }

    private Runnable mTextFieldSelectionListener;

    public void setTextFieldSelectionListener(final Runnable listener) {
        mTextFieldSelectionListener = listener;
    }

    @Override
    protected void onSelectionChanged(final int start, final int end) {
        super.onSelectionChanged(start, end);

        // The listener is installed for the TextFieldEx input adapter.
        // This override can also run during the superclass constructor.
        if (mTextFieldSelectionListener == null) {
            return;
        }

        final Editable text = getText();
        final boolean composing = text != null &&
            BaseInputConnection.getComposingSpanStart(text) >= 0;

        if (start >= 0 && end >= 0 && start != end && !composing) {
            setSelection(end);
            return;
        }

        mTextFieldSelectionListener.run();
    }

    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            ((AxmolActivity)this.getContext()).getGLSurfaceView().requestFocus();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyPreIme(int keyCode, KeyEvent event) {
        return super.onKeyPreIme(keyCode, event);
    }

    public void setInputFlag(int inputFlag) {

        switch (inputFlag) {
            case kEditBoxInputFlagPassword:
                this.mInputFlagConstraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD;
                this.setTypeface(Typeface.DEFAULT);
                this.setTransformationMethod(new PasswordTransformationMethod());
                break;
            case kEditBoxInputFlagSensitive:
                this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
                break;
            case kEditBoxInputFlagInitialCapsWord:
                this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_CAP_WORDS;
                break;
            case kEditBoxInputFlagInitialCapsSentence:
                this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_CAP_SENTENCES;
                break;
            case kEditBoxInputFlagInitialCapsAllCharacters:
                this.mInputFlagConstraints = InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS;
                break;
            case kEditBoxInputFlagLowercaseAllCharacters:
                this.mInputFlagConstraints = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_NORMAL;
                break;
            default:
                break;
        }

        this.setInputType(this.mInputFlagConstraints | this.mInputModeConstraints);
    }

    @Override
    public boolean onTextContextMenuItem(final int id) {
        if (mTextFieldSelectionListener != null &&
            (id == android.R.id.selectAll ||
                id == android.R.id.copy ||
                id == android.R.id.cut)) {
            return true;
        }

        return super.onTextContextMenuItem(id);
    }
}
