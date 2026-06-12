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

 THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
package dev.axmol.lib;

import android.text.Editable;
import android.text.SpannableStringBuilder;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;

/**
 * Custom InputConnection for the Axmol engine.
 * Directly bridges the Android Input Method Framework (IMF) text stream to the C++ layer
 * without relying on a hidden native EditText view container.
 */
public class AxmolInputConnection extends BaseInputConnection {
    public AxmolInputConnection(View targetView, boolean fullEditor) {
        super(targetView, fullEditor);
    }

    /**
     * Invoked when the system fully tears down this connection channel (keyboard goes away).
     */
    @Override
    public void closeConnection() {
        super.closeConnection();
    }

    /**
     * Called by the IME when the user commits text (e.g., presses space or selects a suggestion).
     */
    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {
        if (text == null) return false;
        final String committedStr = text.toString();

        // Dispatch to Axmol GL/Vulkan thread safely to prevent OpenGL context crashes
        AxmolEngine.runOnAxmolThread(() -> {
            AxmolPlayer.nativeInsertText(committedStr);
        });
        return true;
    }

    /**
     * Triggered when the user confirms the composing text (e.g., selects a word from candidate list).
     * Overriding this ensures the custom text pipeline remains completely unblocked within IMF state machines.
     */
    @Override
    public boolean finishComposingText() {
        // Return true to formally signal the IME that the custom field processed the finish request
        return true;
    }

    /**
     * Called by the IME when the user hits the backspace key or requests backward deletion.
     */
    @Override
    public boolean deleteSurroundingText(int beforeLength, int afterLength) {
        if (beforeLength > 0) {
            AxmolEngine.runOnAxmolThread(() -> {
                AxmolPlayer.nativeDeleteBackward(beforeLength);
            });
            return true;
        }
        return super.deleteSurroundingText(beforeLength, afterLength);
    }

    /**
     * Ensure hardware keyboard events or structural soft keys (like backspace/enter/numeric pads)
     * can flow normally down into the target rendering surface view.
     */
    @Override
    public boolean sendKeyEvent(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            int keyCode = event.getKeyCode();
            if (keyCode == KeyEvent.KEYCODE_DEL) {
                AxmolEngine.runOnAxmolThread(() -> {
                    AxmolPlayer.nativeDeleteBackward(1);
                });
                return true;
            }
            if (keyCode == KeyEvent.KEYCODE_ENTER) {
                AxmolEngine.runOnAxmolThread(() -> {
                    AxmolPlayer.nativeInsertText("\n");
                });
                return true;
            }
        }

        // Pass standard view key event dispatching directly to the active Host View
        return super.sendKeyEvent(event);
    }
}
