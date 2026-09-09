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

import android.content.Context;
import android.util.Log;
import android.view.inputmethod.EditorInfo;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 * OpenGL ES implementation of Axmol render surface.
 * Uses GLSurfaceView's built-in render thread mechanism.
 */
public class AxmolSurfaceViewGL extends dev.axmol.lib.GLSurfaceView {
    private static final String TAG = "AxmolSurfaceViewGL";

    private AxmolPlayer mPlayer;

    public AxmolSurfaceViewGL(AxmolPlayer player) {
        super(player.getContext());
        init(player);
    }
    private void init(AxmolPlayer player) {
        mPlayer = player;

        final int[] glContextAttrs = AxmolEngine.nativeGetGLContextAttrs();
        AxmolEGLConfigChooser chooser = new AxmolEGLConfigChooser(glContextAttrs);
        setEGLConfigChooser(chooser);

        setEGLContextClientVersion(3);
        setEGLContextFactory(new AxmolEGLContextFactory());
        setFocusableInTouchMode(true);
        setPreserveEGLContextOnPause(true);

        setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onDrawFrame(GL10 gl) {
                mPlayer.onDrawFrame();
            }

            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                mPlayer.onSurfaceChanged(width, height);
            }

            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                mPlayer.onSurfaceCreated(getHolder().getSurface());
            }
        });

        // Use continuous rendering mode
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    private static class AxmolEGLContextFactory implements GLSurfaceView.EGLContextFactory {
        private static final int EGL_CONTEXT_MAJOR_VERSION_KHR = 0x3098;
        private static final int EGL_CONTEXT_MINOR_VERSION_KHR = 0x30FB;

        @Override
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config) {
            int[] attributes31 = {
                EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
                EGL_CONTEXT_MINOR_VERSION_KHR, 1,
                EGL10.EGL_NONE
            };
            EGLContext context = egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attributes31);
            if (context != null && context != EGL10.EGL_NO_CONTEXT) {
                return context;
            }

            // Clear the failed 3.1 request before applying the GLES 3.0 fallback.
            egl.eglGetError();
            int[] attributes30 = {EGL_CONTEXT_MAJOR_VERSION_KHR, 3, EGL10.EGL_NONE};
            return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attributes30);
        }

        @Override
        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            if (!egl.eglDestroyContext(display, context)) {
                Log.e(TAG, "Failed to destroy EGL context: 0x" + Integer.toHexString(egl.eglGetError()));
            }
        }
    }

    /**
     * Expressly inform the Android IMF that this Player container functions as a text editor,
     * which is absolutely mandatory if we do not rely on an internal EditText instance.
     */
    @Override
    public boolean onCheckIsTextEditor() {
        return mPlayer != null;
    }

    /**
     * Intercepted by Android OS when showSoftInput() is requested.
     * Injects custom input connection and disables landscape fullscreen mode.
     */
    @Override
    public AxmolInputConnection onCreateInputConnection(EditorInfo outAttrs) {
        if (mPlayer != null) {
            return mPlayer.createInputConnection(this, outAttrs);
        }
        return null;
    }

    private class AxmolEGLConfigChooser implements GLSurfaceView.EGLConfigChooser {
        private int[] mConfigAttributes;
        private final int EGL_OPENGL_ES2_BIT = 0x04;
        private final int EGL_OPENGL_ES3_BIT = 0x40;

        public AxmolEGLConfigChooser(int redSize, int greenSize, int blueSize, int alphaSize, int depthSize, int stencilSize, int multisamplingCount) {
            mConfigAttributes = new int[]{redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize, multisamplingCount};
        }

        public AxmolEGLConfigChooser(int[] attributes) {
            mConfigAttributes = attributes;
        }

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            int[][] EGLAttributes = {
                {
                    // GL ES 3 with user set
                    EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                    EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                    EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                    EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                    EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4],
                    EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                    EGL10.EGL_SAMPLE_BUFFERS, (mConfigAttributes[6] > 0) ? 1 : 0,
                    EGL10.EGL_SAMPLES, mConfigAttributes[6],
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                    EGL10.EGL_NONE
                },
                {
                    // GL ES 3 with user set 16 bit depth buffer
                    EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                    EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                    EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                    EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                    EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4] >= 24 ? 16 : mConfigAttributes[4],
                    EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                    EGL10.EGL_SAMPLE_BUFFERS, (mConfigAttributes[6] > 0) ? 1 : 0,
                    EGL10.EGL_SAMPLES, mConfigAttributes[6],
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                    EGL10.EGL_NONE
                },
                {
                    // GL ES 3 with user set 16 bit depth buffer without multisampling
                    EGL10.EGL_RED_SIZE, mConfigAttributes[0],
                    EGL10.EGL_GREEN_SIZE, mConfigAttributes[1],
                    EGL10.EGL_BLUE_SIZE, mConfigAttributes[2],
                    EGL10.EGL_ALPHA_SIZE, mConfigAttributes[3],
                    EGL10.EGL_DEPTH_SIZE, mConfigAttributes[4] >= 24 ? 16 : mConfigAttributes[4],
                    EGL10.EGL_STENCIL_SIZE, mConfigAttributes[5],
                    EGL10.EGL_SAMPLE_BUFFERS, 0,
                    EGL10.EGL_SAMPLES, 0,
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                    EGL10.EGL_NONE
                },
                {
                    // GL ES 3 by default
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                    EGL10.EGL_NONE
                }
            };

            EGLConfig result = null;
            for (int[] eglAtribute : EGLAttributes) {
                result = this.doChooseConfig(egl, display, eglAtribute);
                if (result != null)
                    return result;
            }

            // Some EGL implementations expose GLES 3 contexts on configs that
            // only advertise the legacy ES2 bit. Keep that compatibility path.
            for (int[] eglAttribute : EGLAttributes) {
                int[] fallbackAttributes = eglAttribute.clone();
                for (int i = 0; i + 1 < fallbackAttributes.length; i += 2) {
                    if (fallbackAttributes[i] == EGL10.EGL_RENDERABLE_TYPE) {
                        fallbackAttributes[i + 1] = EGL_OPENGL_ES2_BIT;
                        break;
                    }
                }
                result = this.doChooseConfig(egl, display, fallbackAttributes);
                if (result != null)
                    return result;
            }

            Log.e(Context.DEVICE_POLICY_SERVICE, "Can not select an EGLConfig for rendering.");
            return null;
        }

        private EGLConfig doChooseConfig(EGL10 egl, EGLDisplay display, int[] attributes) {
            EGLConfig[] configs = new EGLConfig[1];
            int[] matchedConfigNum = new int[1];
            boolean result = egl.eglChooseConfig(display, attributes, configs, 1, matchedConfigNum);
            if (result && matchedConfigNum[0] > 0) {
                return configs[0];
            }
            return null;
        }
    }
}
