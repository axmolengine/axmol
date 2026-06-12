/*
 * Copyright (c) 2010-2014 - cocos2d-x community
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 *
 * https://axmol.dev/
 *
 * Portions Copyright (c) Microsoft Open Technologies, Inc.
 * All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "axmol/platform/winrt/EGLSurfaceProvider.h"

#include <winrt/windows.foundation.collections.h>

using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace ax
{

EGLSurfaceProvider::EGLSurfaceProvider() : _eglDisplay(EGL_NO_DISPLAY), _eglContext(EGL_NO_CONTEXT), _eglConfig(nullptr)
{
    initialize();
}

EGLSurfaceProvider::~EGLSurfaceProvider()
{
    cleanup();
}

void EGLSurfaceProvider::initialize()
{
    const EGLint configAttributes[] = {EGL_RED_SIZE,   8, EGL_GREEN_SIZE,   8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
                                       EGL_DEPTH_SIZE, 8, EGL_STENCIL_SIZE, 8, EGL_NONE};

    const EGLint contextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    const EGLint defaultDisplayAttributes[] = {
        // These are the default display attributes, used to request ANGLE's D3D11 renderer.
        // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE,
        EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

        // #ifdef EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER
        //         // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance
        //         benefits on mobile devices.
        //         // Its syntax is subject to change, though. Please update your Visual Studio templates if you
        //         experience compilation issues with it.
        //         EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        // #endif

        // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call
        // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended.
        // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification
        // requirement.
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,
        EGL_TRUE,
        EGL_NONE,
    };

    const EGLint fl9_3DisplayAttributes[] = {
        // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
        // These attributes are used if the call to eglInitialize fails with the default display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE,
        EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE,
        9,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE,
        3,
        // #ifdef EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER
        //         EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        // #endif
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,
        EGL_TRUE,
        EGL_NONE,
    };

    const EGLint warpDisplayAttributes[] = {
        // These attributes can be used to request D3D11 WARP.
        // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE,
        EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE,
        EGL_PLATFORM_ANGLE_DEVICE_TYPE_D3D_WARP_ANGLE,
        // #ifdef EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER
        //         EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        // #endif
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,
        EGL_TRUE,
        EGL_NONE,
    };

    EGLConfig config = NULL;

    // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to
    // configure D3D11.
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (!eglGetPlatformDisplayEXT)
    {
        throw winrt::hresult_error(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
    }

    //
    // To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with
    // varying parameters passed to eglGetPlatformDisplayEXT: 1) The first calls uses "defaultDisplayAttributes" as a
    // parameter. This corresponds to D3D11 Feature Level 10_0+. 2) If eglInitialize fails for step 1 (e.g. because
    // 10_0+ isn't supported by the default GPU), then we try again
    //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
    // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again
    //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software
    //    rasterizer.
    //

    // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
    _eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
    if (_eglDisplay == EGL_NO_DISPLAY)
    {
        throw winrt::hresult_error(E_FAIL, L"Failed to get EGL display");
    }

    if (eglInitialize(_eglDisplay, NULL, NULL) == EGL_FALSE)
    {
        // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile
        // devices).
        _eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
        if (_eglDisplay == EGL_NO_DISPLAY)
        {
            throw winrt::hresult_error(E_FAIL, L"Failed to get EGL display");
        }

        if (eglInitialize(_eglDisplay, NULL, NULL) == EGL_FALSE)
        {
            // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
            _eglDisplay =
                eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
            if (_eglDisplay == EGL_NO_DISPLAY)
            {
                throw winrt::hresult_error(E_FAIL, L"Failed to get EGL display");
            }

            if (eglInitialize(_eglDisplay, NULL, NULL) == EGL_FALSE)
            {
                // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                throw winrt::hresult_error(E_FAIL, L"Failed to initialize EGL");
            }
        }
    }

    EGLint numConfigs = 0;
    if ((eglChooseConfig(_eglDisplay, configAttributes, &_eglConfig, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
    {
        throw winrt::hresult_error(E_FAIL, L"Failed to choose first EGLConfig");
    }

    _eglContext = eglCreateContext(_eglDisplay, _eglConfig, EGL_NO_CONTEXT, contextAttributes);
    if (_eglContext == EGL_NO_CONTEXT)
    {
        throw winrt::hresult_error(E_FAIL, L"Failed to create EGL context");
    }
}

void EGLSurfaceProvider::cleanup()
{
    if (_eglDisplay != EGL_NO_DISPLAY && _eglContext != EGL_NO_CONTEXT)
    {
        eglDestroyContext(_eglDisplay, _eglContext);
        _eglContext = EGL_NO_CONTEXT;
    }

    if (_eglDisplay != EGL_NO_DISPLAY)
    {
        eglTerminate(_eglDisplay);
        _eglDisplay = EGL_NO_DISPLAY;
    }
}

void EGLSurfaceProvider::reset()
{
    cleanup();
    initialize();
}

EGLSurface EGLSurfaceProvider::createSurface(SwapChainPanel const& panel,
                                             const Windows::Foundation::Size* renderSurfaceSize,
                                             const float* resolutionScale)
{
    if (!panel)
    {
        throw winrt::hresult_error(E_INVALIDARG, L"SwapChainPanel parameter is invalid");
    }

    if (renderSurfaceSize != nullptr && resolutionScale != nullptr)
    {
        throw winrt::hresult_error(E_INVALIDARG, L"A size and a scale can't both be specified");
    }

    EGLSurface surface = EGL_NO_SURFACE;

    const EGLint surfaceAttributes[] = {// EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as
                                        // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above). If you have
                                        // compilation issues with it then please update your Visual Studio templates.
                                        // #if defined(EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER)
                                        //        EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
                                        // #endif
                                        EGL_NONE};

    // Create a PropertySet and initialize with the EGLNativeWindowType.
    PropertySet surfaceCreationProperties = PropertySet();
    surfaceCreationProperties.Insert(winrt::hstring(EGLNativeWindowTypeProperty), panel);

    // If a render surface size is specified, add it to the surface creation properties
    if (renderSurfaceSize != nullptr)
    {
        surfaceCreationProperties.Insert(winrt::hstring(EGLRenderSurfaceSizeProperty),
                                         PropertyValue::CreateSize(*renderSurfaceSize));
    }

    // If a resolution scale is specified, add it to the surface creation properties
    if (resolutionScale != nullptr)
    {
        surfaceCreationProperties.Insert(winrt::hstring(EGLRenderResolutionScaleProperty),
                                         PropertyValue::CreateSingle(*resolutionScale));
    }

    auto native_abi = winrt::get_abi(surfaceCreationProperties);
    surface = eglCreateWindowSurface(_eglDisplay, _eglConfig, (EGLNativeWindowType)native_abi, surfaceAttributes);
    if (surface == EGL_NO_SURFACE)
    {
        throw winrt::hresult_error(E_FAIL, L"Failed to create EGL surface");
    }

    return surface;
}

void EGLSurfaceProvider::getSurfaceDimensions(const EGLSurface surface, EGLint* width, EGLint* height)
{
    eglQuerySurface(_eglDisplay, surface, EGL_WIDTH, width);
    eglQuerySurface(_eglDisplay, surface, EGL_HEIGHT, height);
}

void EGLSurfaceProvider::destroySurface(const EGLSurface surface)
{
    if (_eglDisplay != EGL_NO_DISPLAY && surface != EGL_NO_SURFACE)
    {
        eglDestroySurface(_eglDisplay, surface);
    }
}

void EGLSurfaceProvider::makeCurrent(const EGLSurface surface)
{
    if (eglMakeCurrent(_eglDisplay, surface, surface, _eglContext) == EGL_FALSE)
    {
        throw winrt::hresult_error(E_FAIL, L"Failed to make EGLSurface current");
    }
}

EGLBoolean EGLSurfaceProvider::swapBuffers(const EGLSurface surface)
{
    return (eglSwapBuffers(_eglDisplay, surface));
}

}  // namespace ax
