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
package org.axmol.lib;

import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.graphics.Rect;
import android.media.AudioManager;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.IBinder;
import android.os.ParcelFileDescriptor;
import android.os.Vibrator;
import android.preference.PreferenceManager.OnActivityResultListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.DisplayCutout;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;

import com.android.vending.expansion.zipfile.APKExpansionSupport;
import com.android.vending.expansion.zipfile.ZipResourceFile;

import com.jakewharton.processphoenix.ProcessPhoenix;

import java.io.IOException;
import java.io.File;
import java.io.FilenameFilter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;


public class AxmolEngine {
    // ===========================================================
    // Constants
    // ===========================================================
    private static final String PREFS_NAME = "AXPrefsFile";
    private static final int RUNNABLES_PER_FRAME = 5;
    private static final String TAG = AxmolEngine.class.getSimpleName();

    // ===========================================================
    // Fields
    // ===========================================================
    private static AssetManager sAssetManager;
    private static AxmolAccelerometer sAccelerometer = null;
    private static boolean sAccelerometerEnabled;
    private static boolean sCompassEnabled;
    private static boolean sActivityVisible;
    private static String sPackageName;
    private static Activity sActivity = null;
    private static AxmolEngineListener sAxmolEngineListener;
    private static Set<OnActivityResultListener> onActivityResultListeners = new LinkedHashSet<OnActivityResultListener>();
    private static Vibrator sVibrateService = null;

    // The absolute path to the OBB if it exists, else the absolute path to the APK.
    private static String sAssetsPath = "";

    // The OBB file
    private static ZipResourceFile sOBBFile = null;

    // ===========================================================
    // Constructors
    // ===========================================================

    public static void runOnGLThread(final Runnable r) {
        nativeRunOnGLThread(r);
    }

    public static void queueOperation(final long op, final long param) {
        ((AxmolActivity)sActivity).getGLSurfaceView().queueEvent(new Runnable() {
            @Override
            public void run() {
                AxmolEngine.nativeCall0(op, param);
            }
        });
    }

    private static boolean sInited = false;
    public static void init(final Activity activity) {
        sActivity = activity;
        AxmolEngine.sAxmolEngineListener = (AxmolEngineListener)activity;
        if (!sInited) {

            PackageManager pm = activity.getPackageManager();
            boolean isSupportLowLatency = pm.hasSystemFeature(PackageManager.FEATURE_AUDIO_LOW_LATENCY);
            Log.d(TAG, String.format("android version is %d, isSupportLowLatency: %s", Build.VERSION.SDK_INT, isSupportLowLatency ? "true" : "false") );

            final ApplicationInfo applicationInfo = activity.getApplicationInfo();

            AxmolEngine.sPackageName = applicationInfo.packageName;

            AxmolEngine.sAssetManager = activity.getAssets();
            AxmolEngine.nativeSetContext((Context)activity, AxmolEngine.sAssetManager);

            AxmolMediaEngine.setContext(activity);

            BitmapHelper.setContext(activity);

            AxmolEngine.sVibrateService = (Vibrator)activity.getSystemService(Context.VIBRATOR_SERVICE);

            sInited = true;
        }
    }

    // This function returns the absolute path to the OBB if it exists,
    // else it returns the absolute path to the APK.
    public static String getAssetsPath()
    {
        if (AxmolEngine.sAssetsPath.equals("")) {

            String pathToOBB = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/obb/" + AxmolEngine.sPackageName;

	    	// Listing all files inside the folder (pathToOBB) where OBB files are expected to be found.
            String[] fileNames = new File(pathToOBB).list(new FilenameFilter() { // Using filter to pick up only main OBB file name.
                public boolean accept(File dir, String name) {
                    return name.startsWith("main.") && name.endsWith(".obb");  // It's possible to filter only by extension here to get path to patch OBB file also.
                }
            });

            String fullPathToOBB = "";
            if (fileNames != null && fileNames.length > 0)  // If there is at least 1 element inside the array with OBB file names, then we may think fileNames[0] will have desired main OBB file name.
                fullPathToOBB = pathToOBB + "/" + fileNames[0];  // Composing full file name for main OBB file.

            File obbFile = new File(fullPathToOBB);
            if (obbFile.exists())
                AxmolEngine.sAssetsPath = fullPathToOBB;
            else
                AxmolEngine.sAssetsPath = AxmolEngine.sActivity.getApplicationInfo().sourceDir;
        }

        return AxmolEngine.sAssetsPath;
    }

    public static ZipResourceFile getObbFile() {
        if (null == sOBBFile) {
            int versionCode = 1;
            try {
                versionCode = AxmolActivity.getContext().getPackageManager().getPackageInfo(AxmolEngine.getPackageName(), 0).versionCode;
            } catch (NameNotFoundException e) {
                e.printStackTrace();
            }

            try {
                sOBBFile = APKExpansionSupport.getAPKExpansionZipFile(AxmolActivity.getContext(), versionCode, 0);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        return sOBBFile;
    }

    public static Activity getActivity() {
        return sActivity;
    }

    public static void addOnActivityResultListener(OnActivityResultListener listener) {
        onActivityResultListeners.add(listener);
    }

    public static Set<OnActivityResultListener> getOnActivityResultListeners() {
        return onActivityResultListeners;
    }

    public static boolean isActivityVisible(){
        return sActivityVisible;
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeRunOnGLThread(final Object runnable);

    private static native void nativeSetEditTextDialogResult(final byte[] pBytes);

    private static native void nativeSetContext(final Object pContext, final Object pAssetManager);

    // private static native void nativeSetAudioDeviceInfo(boolean isSupportLowLatency, int deviceSampleRate, int audioBufferSizeInFames);

    public static native void nativeCall0(long func, long ud);

    public static String getPackageName() {
        return AxmolEngine.sPackageName;
    }
    public static String getWritablePath() {
        return sActivity.getFilesDir().getAbsolutePath();
    }

    public static String getCurrentLanguage() {
        return Locale.getDefault().getLanguage();
    }

    public static String getDeviceModel(){
        return Build.MODEL;
    }

    public static AssetManager getAssetManager() {
        return AxmolEngine.sAssetManager;
    }

    public static void enableAccelerometer() {
        AxmolEngine.sAccelerometerEnabled = true;
        AxmolEngine.getAccelerometer().enableAccel();
    }

    public static void enableCompass() {
        AxmolEngine.sCompassEnabled = true;
        AxmolEngine.getAccelerometer().enableCompass();
    }

    public static void setAccelerometerInterval(float interval) {
        AxmolEngine.getAccelerometer().setInterval(interval);
    }

    public static void disableAccelerometer() {
        AxmolEngine.sAccelerometerEnabled = false;
        AxmolEngine.getAccelerometer().disable();
    }

    public static void setKeepScreenOn(boolean value) {
        ((AxmolActivity)sActivity).setKeepScreenOn(value);
    }

    public static void vibrate(float duration) {
        sVibrateService.vibrate((long)(duration * 1000));
    }

    public static void impactOccurred(int style) {
        ((AxmolActivity)sActivity).impactOccurred(style);
    }

    public static void notificationOccurred(int type) {
        ((AxmolActivity)sActivity).notificationOccurred(type);
    }

    public static void selectionChanged() {
        ((AxmolActivity)sActivity).selectionChanged();
    }

 	public static String getVersion() {
 		try {
 			String version = AxmolActivity.getContext().getPackageManager().getPackageInfo(AxmolActivity.getContext().getPackageName(), 0).versionName;
 			return version;
 		} catch(Exception e) {
 			return "";
 		}
 	}

    public static boolean openURL(String url) {
        boolean ret = false;
        try {
            Intent i = new Intent(Intent.ACTION_VIEW);
            i.setData(Uri.parse(url));
            sActivity.startActivity(i);
            ret = true;
        } catch (Exception e) {
        }
        return ret;
    }

    public static long[] getObbAssetFileDescriptor(final String path) {
        long[] array = new long[3];
        if (AxmolEngine.getObbFile() != null) {
            AssetFileDescriptor descriptor = AxmolEngine.getObbFile().getAssetFileDescriptor(path);
            if (descriptor != null) {
                try {
                    ParcelFileDescriptor parcel = descriptor.getParcelFileDescriptor();
                    Method method = parcel.getClass().getMethod("getFd", new Class[] {});
                    array[0] = (Integer)method.invoke(parcel);
                    array[1] = descriptor.getStartOffset();
                    array[2] = descriptor.getLength();
                } catch (NoSuchMethodException e) {
                    Log.e(AxmolEngine.TAG, "Accessing file descriptor directly from the OBB is only supported from Android 3.1 (API level 12) and above.");
                } catch (IllegalAccessException e) {
                    Log.e(AxmolEngine.TAG, e.toString());
                } catch (InvocationTargetException e) {
                    Log.e(AxmolEngine.TAG, e.toString());
                }
            }
        }
        return array;
    }

    public static void onResume() {
        sActivityVisible = true;
        if (AxmolEngine.sAccelerometerEnabled) {
            AxmolEngine.getAccelerometer().enableAccel();
        }
        if (AxmolEngine.sCompassEnabled) {
            AxmolEngine.getAccelerometer().enableCompass();
        }
    }

    public static void onPause() {
        sActivityVisible = false;
        if (AxmolEngine.sAccelerometerEnabled) {
            AxmolEngine.getAccelerometer().disable();
        }
    }

    public static void onExit() {
        // Remove it from recent apps.
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            sActivity.finishAndRemoveTask();
        }
    }

    private static void showDialog(final String pTitle, final String pMessage) {
        AxmolEngine.sAxmolEngineListener.showDialog(pTitle, pMessage);
    }


    public static void setEditTextDialogResult(final String pResult) {
        try {
            final byte[] bytesUTF8 = pResult.getBytes("UTF8");

            AxmolEngine.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    AxmolEngine.nativeSetEditTextDialogResult(bytesUTF8);
                }
            });
        } catch (UnsupportedEncodingException pUnsupportedEncodingException) {
            /* Nothing. */
        }
    }

    private static int displayMetricsToDPI(DisplayMetrics metrics)
    {
        return metrics.densityDpi;
    }

    public static int getDPI()
    {
        if (sActivity != null)
        {
            DisplayMetrics metrics = new DisplayMetrics();
            WindowManager wm = sActivity.getWindowManager();
            if (wm != null)
            {
                Display d = wm.getDefaultDisplay();
                if (d != null)
                {
                    try {
                        Method getRealMetrics = d.getClass().getMethod("getRealMetrics", metrics.getClass());
                        getRealMetrics.invoke(d, metrics);
                        return displayMetricsToDPI(metrics);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    d.getMetrics(metrics);
                    return displayMetricsToDPI(metrics);
                }
            }
        }
        return -1;
    }

    // ===========================================================
    // Functions for CCUserDefault
    // ===========================================================

    public static boolean getBoolForKey(String key, boolean defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        try {
            return settings.getBoolean(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            Map allValues = settings.getAll();
            Object value = allValues.get(key);
            if ( value instanceof String)
            {
                return  Boolean.parseBoolean(value.toString());
            }
            else if (value instanceof Integer)
            {
                int intValue = ((Integer) value).intValue();
                return (intValue !=  0) ;
            }
            else if (value instanceof Float)
            {
                float floatValue = ((Float) value).floatValue();
                return (floatValue != 0.0f);
            }
        }

        return defaultValue;
    }

    public static int getIntegerForKey(String key, int defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        try {
            return settings.getInt(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            Map allValues = settings.getAll();
            Object value = allValues.get(key);
            if ( value instanceof String) {
                return  Integer.parseInt(value.toString());
            }
            else if (value instanceof Float)
            {
                return ((Float) value).intValue();
            }
            else if (value instanceof Boolean)
            {
                boolean booleanValue = ((Boolean) value).booleanValue();
                if (booleanValue)
                    return 1;
            }
        }

        return defaultValue;
    }

    public static float getFloatForKey(String key, float defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        try {
            return settings.getFloat(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            Map allValues = settings.getAll();
            Object value = allValues.get(key);
            if ( value instanceof String) {
                return  Float.parseFloat(value.toString());
            }
            else if (value instanceof Integer)
            {
                return ((Integer) value).floatValue();
            }
            else if (value instanceof Boolean)
            {
                boolean booleanValue = ((Boolean) value).booleanValue();
                if (booleanValue)
                    return 1.0f;
            }
        }

        return defaultValue;
    }

    public static double getDoubleForKey(String key, double defaultValue) {
        // SharedPreferences doesn't support saving double value
        return getFloatForKey(key, (float) defaultValue);
    }

    public static String getStringForKey(String key, String defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        try {
            return settings.getString(key, defaultValue);
        }
        catch (Exception ex) {
            ex.printStackTrace();

            return settings.getAll().get(key).toString();
        }
    }

    public static void setBoolForKey(String key, boolean value) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(key, value);
        editor.apply();
    }

    public static void setIntegerForKey(String key, int value) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt(key, value);
        editor.apply();
    }

    public static void setFloatForKey(String key, float value) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, value);
        editor.apply();
    }

    public static void setDoubleForKey(String key, double value) {
        // SharedPreferences doesn't support recording double value
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, (float)value);
        editor.apply();
    }

    public static void setStringForKey(String key, String value) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(key, value);
        editor.apply();
    }

    public static void deleteValueForKey(String key) {
        SharedPreferences settings = sActivity.getSharedPreferences(AxmolEngine.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.remove(key);
        editor.apply();
    }

    public static byte[] conversionEncoding(byte[] text, String fromCharset,String newCharset)
    {
        try {
            String str = new String(text,fromCharset);
            return str.getBytes(newCharset);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        return null;
    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public static interface AxmolEngineListener {
        public void showDialog(final String pTitle, final String pMessage);
    }

    /**
     * Returns whether the screen has a round shape. Apps may choose to change styling based
     * on this property, such as the alignment or layout of text or informational icons.
     *
     * @return true if the screen is rounded, false otherwise
     */
    public static boolean isScreenRound() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (sActivity.getResources().getConfiguration().isScreenRound()) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns whether the window is always allowed to extend into the DisplayCutout areas on the short edges of the screen.
     *
     * @return true if the window in display cutout mode on the short edges of the screen, false otherwise
     */
    @SuppressLint("InlinedApi")
    public static boolean isCutoutEnabled() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowManager.LayoutParams lp = sActivity.getWindow().getAttributes();
            return lp.layoutInDisplayCutoutMode == WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }

        return false;
    }

    /**
     * Returns safe insets array.
     *
     * @return array of int with safe insets values
     */
    @SuppressLint("NewApi")
    public static int[] getSafeInsets() {
        final int[] safeInsets = new int[]{0, 0, 0, 0};
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            Window cocosWindow = sActivity.getWindow();
            DisplayCutout displayCutout = cocosWindow.getDecorView().getRootWindowInsets().getDisplayCutout();
            // Judge whether it is cutouts (aka notch) screen phone by judge cutout equle to null
            if (displayCutout != null) {
                List<Rect> rects = displayCutout.getBoundingRects();
                // Judge whether it is cutouts (aka notch) screen phone by judge cutout rects is null or zero size
                if (rects != null && rects.size() != 0) {
                    safeInsets[0] = displayCutout.getSafeInsetBottom();
                    safeInsets[1] = displayCutout.getSafeInsetLeft();
                    safeInsets[2] = displayCutout.getSafeInsetRight();
                    safeInsets[3] = displayCutout.getSafeInsetTop();
                }
            }
        }

        return safeInsets;
    }

    /**
     * Returns rounded corner radius array.
     *
     * @return array of int with rounded corner radius values
     */
    @SuppressLint("NewApi")
    public static int[] getDeviceCornerRadii() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            final int[] radii = new int[]{0, 0, 0, 0};
            Window cocosWindow = sActivity.getWindow();
            View view = cocosWindow.getDecorView();
            WindowInsets insets = view.getRootWindowInsets();
            android.view.RoundedCorner topLeft = insets.getRoundedCorner(android.view.RoundedCorner.POSITION_TOP_LEFT);
            android.view.RoundedCorner topRight = insets.getRoundedCorner(android.view.RoundedCorner.POSITION_TOP_RIGHT);
            android.view.RoundedCorner bottomLeft = insets.getRoundedCorner(android.view.RoundedCorner.POSITION_BOTTOM_LEFT);
            android.view.RoundedCorner bottomRight = insets.getRoundedCorner(android.view.RoundedCorner.POSITION_BOTTOM_RIGHT);
            int radiusTopLeft = 0;
            int radiusTopRight = 0;
            int radiusBottomLeft = 0;
            int radiusBottomRight = 0;
            if (topLeft != null) radiusTopLeft = topLeft.getRadius();
            if (topRight != null) radiusTopRight = topRight.getRadius();
            if (bottomLeft != null) radiusBottomLeft = bottomLeft.getRadius();
            if (bottomRight != null) radiusBottomRight = bottomRight.getRadius();

            int leftRadius = Math.max(radiusTopLeft, radiusBottomLeft);
            int topRadius = Math.max(radiusTopLeft, radiusTopRight);
            int rightRadius = Math.max(radiusTopRight, radiusBottomRight);
            int bottomRadius = Math.max(radiusBottomLeft, radiusBottomRight);

            radii[0] = bottomRadius;
            radii[1] = leftRadius;
            radii[2] = rightRadius;
            radii[3] = topRadius;
            return radii;
        }

        return null;
    }

    /**
     * Queries about whether any physical keys exist on the
     * any keyboard attached to the device and returns <code>true</code>
     * if the device does not have physical keys
     *
     * @return Returns <code>true</code> if the device have no physical keys,
     * otherwise <code>false</code> will returned.
     */
    public static boolean hasSoftKeys() {
        boolean hasSoftwareKeys = true;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            Display display = sActivity.getWindowManager().getDefaultDisplay();

            DisplayMetrics realDisplayMetrics = new DisplayMetrics();
            display.getRealMetrics(realDisplayMetrics);

            int realHeight = realDisplayMetrics.heightPixels;
            int realWidth = realDisplayMetrics.widthPixels;

            DisplayMetrics displayMetrics = new DisplayMetrics();
            display.getMetrics(displayMetrics);

            int displayHeight = displayMetrics.heightPixels;
            int displayWidth = displayMetrics.widthPixels;

            hasSoftwareKeys = (realWidth - displayWidth) > 0 ||
                    (realHeight - displayHeight) > 0;
        } else {
            boolean hasMenuKey = ViewConfiguration.get(sActivity).hasPermanentMenuKey();
            boolean hasBackKey = KeyCharacterMap.deviceHasKey(KeyEvent.KEYCODE_BACK);
            hasSoftwareKeys = !hasMenuKey && !hasBackKey;
        }
        return hasSoftwareKeys;
    }

    public static float[] getAccelValue() {
        return AxmolEngine.getAccelerometer().accelerometerValues;
    }

    public static float[] getCompassValue() {
        return AxmolEngine.getAccelerometer().compassFieldValues;
    }

    public static int getSDKVersion() {
        return Build.VERSION.SDK_INT;
    }

    public static void restartProcess() {
        ProcessPhoenix.triggerRebirth(sActivity);
    }

    private static AxmolAccelerometer getAccelerometer() {
        if (null == sAccelerometer)
            AxmolEngine.sAccelerometer = new AxmolAccelerometer(sActivity);

        return sAccelerometer;
    }
}
