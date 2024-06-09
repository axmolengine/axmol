/****************************************************************************
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

package org.axmol.lib;

import android.annotation.TargetApi;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.util.SparseArray;
import android.view.View;
import android.webkit.WebView;
import android.widget.FrameLayout;
import android.webkit.WebSettings;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;


public class WebViewHelper {
    private static final String TAG = WebViewHelper.class.getSimpleName();
    private static Handler sHandler;
    private static AxmolActivity sAxmolActivity;
    private static FrameLayout sLayout;

    private static SparseArray<AxmolWebView> webViews;
    private static int viewTag = 0;

    public WebViewHelper(FrameLayout layout) {
        WebViewHelper.sLayout = layout;
        WebViewHelper.sHandler = new Handler(Looper.myLooper());

        WebViewHelper.sAxmolActivity = (AxmolActivity) AxmolActivity.getContext();
        WebViewHelper.webViews = new SparseArray<AxmolWebView>();
    }

    private static native boolean shouldStartLoading(int index, String message);

    public static boolean _shouldStartLoading(int index, String message) {
        return !shouldStartLoading(index, message);
    }

    private static native void didFinishLoading(int index, String message);

    public static void _didFinishLoading(int index, String message) {
        didFinishLoading(index, message);
    }

    private static native void didFailLoading(int index, String message);

    public static void _didFailLoading(int index, String message) {
        didFailLoading(index, message);
    }

    private static native void onJsCallback(int index, String message);

    public static void _onJsCallback(int index, String message) {
        onJsCallback(index, message);
    }

    public static int createWebView() {
        final int index = viewTag;
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = new AxmolWebView(sAxmolActivity, index);
                FrameLayout.LayoutParams lParams = new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.WRAP_CONTENT,
                        FrameLayout.LayoutParams.WRAP_CONTENT);
                sLayout.addView(webView, lParams);

                webViews.put(index, webView);
            }
        });
        return viewTag++;
    }

    public static void removeWebView(final int index) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webViews.remove(index);
                    sLayout.removeView(webView);
                }
            }
        });
    }

    public static void setVisible(final int index, final boolean visible) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.setVisibility(visible ? View.VISIBLE : View.GONE);
                }
            }
        });
    }

    public static void setBackgroundTransparent(final int index) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.setBackgroundColor(Color.TRANSPARENT);
                }
            }
        });
    }

    public static void setOpacityWebView(final int index, final float opacity) {
        if(android.os.Build.VERSION.SDK_INT >10){
            sAxmolActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    AxmolWebView webView = webViews.get(index);
                    if (webView != null) {
                        try {
                            Method method = webView.getClass().getMethod("setAlpha",float.class);
                            method.invoke(webView,opacity);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
            });
        }
    }


    public static float getOpacityWebView(final int index) {
        if(android.os.Build.VERSION.SDK_INT >10){
            FutureTask<Float> futureResult = new FutureTask<Float>(new Callable<Float>() {
            @Override
            public Float call() throws Exception {
                float opacity=0.f;
                AxmolWebView webView = webViews.get(index);
                Object valueToReturn=null;
                if (webView != null) {
                    try {
                        Method method = webView.getClass().getMethod("getAlpha");
                        valueToReturn = method.invoke(webView);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                return (Float) valueToReturn;
            }
            });
            sAxmolActivity.runOnUiThread(futureResult);
            try {
                return futureResult.get();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return 1;
    }

    public static void setWebViewRect(final int index, final int left, final int top, final int maxWidth, final int maxHeight) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.setWebViewRect(left, top, maxWidth, maxHeight);
                }
            }
        });
    }

    public static void setJavascriptInterfaceScheme(final int index, final String scheme) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.setJavascriptInterfaceScheme(scheme);
                }
            }
        });
    }

    public static void loadData(final int index, final String data, final String mimeType, final String encoding, final String baseURL) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.loadDataWithBaseURL(baseURL, data, mimeType, encoding, null);
                }
            }
        });
    }

    public static void loadHTMLString(final int index, final String data, final String baseUrl) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.loadDataWithBaseURL(baseUrl, data, null, null, null);
                }
            }
        });
    }

    public static void loadUrl(final int index, final String url, final boolean cleanCachedData) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.getSettings().setCacheMode(cleanCachedData ? WebSettings.LOAD_NO_CACHE
                                                                       : WebSettings.LOAD_DEFAULT);
                    webView.loadUrl(url);
                }
            }
        });
    }

    public static void loadFile(final int index, final String filePath) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.loadUrl(filePath);
                }
            }
        });
    }

    public static void stopLoading(final int index) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.stopLoading();
                }
            }
        });

    }

    public static void reload(final int index) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.reload();
                }
            }
        });
    }

    public static <T> T callInMainThread(Callable<T> call) throws ExecutionException, InterruptedException {
        FutureTask<T> task = new FutureTask<T>(call);
        sHandler.post(task);
        return task.get();
    }

    public static boolean canGoBack(final int index) {
        Callable<Boolean> callable = new Callable<Boolean>() {
            @Override
            public Boolean call() throws Exception {
                AxmolWebView webView = webViews.get(index);
                return webView != null && webView.canGoBack();
            }
        };
        try {
            return callInMainThread(callable);
        } catch (ExecutionException e) {
            return false;
        } catch (InterruptedException e) {
            return false;
        }
    }

    public static boolean canGoForward(final int index) {
        Callable<Boolean> callable = new Callable<Boolean>() {
            @Override
            public Boolean call() throws Exception {
                AxmolWebView webView = webViews.get(index);
                return webView != null && webView.canGoForward();
            }
        };
        try {
            return callInMainThread(callable);
        } catch (ExecutionException e) {
            return false;
        } catch (InterruptedException e) {
            return false;
        }
    }

    public static void goBack(final int index) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.goBack();
                }
            }
        });
    }

    public static void goForward(final int index) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.goForward();
                }
            }
        });
    }

    public static void evaluateJS(final int index, final String js) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.loadUrl("javascript:" + js);
                }
            }
        });
    }

    public static void setScalesPageToFit(final int index, final boolean scalesPageToFit) {
        sAxmolActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AxmolWebView webView = webViews.get(index);
                if (webView != null) {
                    webView.setScalesPageToFit(scalesPageToFit);
                }
            }
        });
    }
}
