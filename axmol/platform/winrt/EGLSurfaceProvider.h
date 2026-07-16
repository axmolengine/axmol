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

#pragma once

// OpenGL ES includes
#include <GLES3/gl3.h>

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <angle_windowsstore.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

using namespace winrt;

namespace ax
{

class EGLSurfaceProvider
{
public:
    EGLSurfaceProvider();
    ~EGLSurfaceProvider();

    EGLSurface createSurface(Windows::UI::Xaml::Controls::SwapChainPanel const& panel,
                             const Windows::Foundation::Size* renderSurfaceSize,
                             const float* renderResolutionScale);
    void getSurfaceDimensions(const EGLSurface surface, EGLint* width, EGLint* height);
    void destroySurface(const EGLSurface surface);
    void makeCurrent(const EGLSurface surface);
    EGLBoolean swapBuffers(const EGLSurface surface);
    void reset();
    void cleanup();

private:
    void initialize();

private:
    EGLDisplay _eglDisplay;
    EGLContext _eglContext;
    EGLConfig _eglConfig;
};
}  // namespace ax
