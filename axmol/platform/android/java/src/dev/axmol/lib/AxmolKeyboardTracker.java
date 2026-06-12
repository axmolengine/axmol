/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/
 ****************************************************************************/
package dev.axmol.lib;

import android.annotation.TargetApi;
import android.graphics.Insets;
import android.graphics.Rect;
import android.os.Build;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsAnimation;
import java.util.List;

/**
 * Unified Keyboard Lifecycle and Geometry Tracker for Axmol Engine.
 * Automatically routes between modern WindowInsets (API 30+) and Legacy Layout fallbacks (API 21+).
 */
public class AxmolKeyboardTracker {

    private static boolean mIsKeyboardVisible = false;
    private static int mPreviousHeight = 0;

    /**
     * Primary entry point. Register this on your root rendering window view.
     * Fully safe to invoke across all API levels (21 to 30+).
     */
    public static void register(final View targetView) {
        if (targetView == null) return;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            // Android 11+ (API 30+) High-performance path
            Api30Impl.register(targetView);
        } else {
            // Android 5.0 - 10.0 (API 21-29) Legacy fallback path
            registerLegacyTracker(targetView);
        }
    }

    /**
     * Legacy Fallback subsystem using traditional Global Layout Listeners.
     * Kept inside the outer class as these APIs are safe down to API 1.
     */
    private static void registerLegacyTracker(final View targetView) {
        targetView.getViewTreeObserver().addOnGlobalLayoutListener(() -> {
            Rect visibleRect = new Rect();
            targetView.getWindowVisibleDisplayFrame(visibleRect);

            int totalScreenHeight = targetView.getRootView().getHeight();
            int activeKeyboardHeight = totalScreenHeight - visibleRect.bottom;

            // Standard safety check threshold (15% of screen height)
            boolean isCurrentlyVisible = activeKeyboardHeight > (totalScreenHeight * 0.15);

            if (isCurrentlyVisible) {
                if (!mIsKeyboardVisible || activeKeyboardHeight != mPreviousHeight) {
                    mIsKeyboardVisible = true;
                    mPreviousHeight = activeKeyboardHeight;

                    final int finalHeight = activeKeyboardHeight;
                    AxmolEngine.runOnAxmolThread(() ->
                        AxmolPlayer.nativeSoftInputShow(
                            visibleRect.left, visibleRect.bottom, visibleRect.width(), finalHeight, 0.25f
                        )
                    );
                }
            } else {
                if (mIsKeyboardVisible) {
                    mIsKeyboardVisible = false;
                    mPreviousHeight = 0;
                    AxmolEngine.runOnAxmolThread(() -> AxmolPlayer.nativeSoftInputHide(0.25f));
                }
            }
        });
    }

    /**
     * Internal container isolating Android 11 (API 30) types.
     * This prevents older Android 5.0 runtimes from crashing due to unresolved Class References.
     */
    @TargetApi(Build.VERSION_CODES.R)
    private static class Api30Impl {
        private static float mAnimationDuration = 0.25f;

        static void register(final View targetView) {
            // 1. Hook into the IME's underlying physics clock
            targetView.setWindowInsetsAnimationCallback(new WindowInsetsAnimation.Callback(
                    WindowInsetsAnimation.Callback.DISPATCH_MODE_STOP) {
                @Override
                public void onPrepare(WindowInsetsAnimation animation) {
                    if ((animation.getTypeMask() & WindowInsets.Type.ime()) != 0) {
                        mAnimationDuration = animation.getDurationMillis() / 1000f;
                        if (mAnimationDuration <= 0) mAnimationDuration = 0.25f;
                    }
                }

                @Override
                public WindowInsetsAnimation.Bounds onStart(WindowInsetsAnimation anim, WindowInsetsAnimation.Bounds bounds) {
                    return bounds;
                }

                @Override
                public WindowInsets onProgress(WindowInsets insets, List<WindowInsetsAnimation> anims) {
                    return insets;
                }
            });

            // 2. Extract pixel-perfect bounds without system navigation bar interference
            targetView.setOnApplyWindowInsetsListener((v, insets) -> {
                Insets imeInsets = insets.getInsets(WindowInsets.Type.ime());
                Insets systemBars = insets.getInsets(WindowInsets.Type.systemBars());

                int keyboardHeight = imeInsets.bottom - systemBars.bottom;
                boolean isVisible = insets.isVisible(WindowInsets.Type.ime());

                Rect visibleRect = new Rect();
                targetView.getWindowVisibleDisplayFrame(visibleRect);

                if (isVisible && keyboardHeight > 0) {
                    if (!mIsKeyboardVisible || keyboardHeight != mPreviousHeight) {
                        mIsKeyboardVisible = true;
                        mPreviousHeight = keyboardHeight;

                        final int finalHeight = keyboardHeight;
                        final float currentDuration = mAnimationDuration;
                        AxmolEngine.runOnAxmolThread(() ->
                            AxmolPlayer.nativeSoftInputShow(
                                visibleRect.left, visibleRect.bottom, visibleRect.width(), finalHeight, currentDuration
                            )
                        );
                    }
                } else {
                    if (mIsKeyboardVisible) {
                        mIsKeyboardVisible = false;
                        mPreviousHeight = 0;

                        final float currentDuration = mAnimationDuration;
                        AxmolEngine.runOnAxmolThread(() -> AxmolPlayer.nativeSoftInputHide(currentDuration));
                    }
                }
                return v.onApplyWindowInsets(insets);
            });
        }
    }
}
