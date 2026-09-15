/****************************************************************************
Copyright (c) 2015-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.
 
https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
package dev.axmol.app;

import android.os.Bundle;
import dev.axmol.lib.AxmolActivity;
import dev.axmol.lib.SharedLoader;
import android.os.Build;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;

public class AppActivity extends AxmolActivity {
    static {
        // DNT remove, some android simulator require explicit load shared libraries, otherwise will crash
        SharedLoader.load();
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.setEnableVirtualButton(false);
        super.onCreate(savedInstanceState);
        // Workaround in https://stackoverflow.com/questions/16283079/re-launch-of-activity-on-home-button-but-only-the-first-time/16447508
        if (!isTaskRoot()) {
            // Android launched another instance of the root activity into an existing task
            //  so just quietly finish and go away, dropping the user back into the activity
            //  at the top of the stack (ie: the last state of this task)
            // Don't need to finish it again since it's finished in super.onCreate .
            return;
        }
        // Make sure we're running on Pie or higher to change cutout mode
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            // Enable rendering into the cutout area
            WindowManager.LayoutParams lp = getWindow().getAttributes();
            lp.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            getWindow().setAttributes(lp);
        }
        // DO OTHER INITIALIZATION BELOW
        
    }

}
