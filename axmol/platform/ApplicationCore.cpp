/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/platform/ApplicationCore.h"
#include "axmol/base/Director.h"
#include "axmol/platform/CommandLineArgs.h"

namespace ax
{

ContextAttrs ApplicationCore::s_contextAttrs = ContextAttrs{};

Application* ApplicationCore::s_axmolApp = nullptr;
Director* ApplicationCore::s_director    = nullptr;

static DriverPreference parseDriverPreference(std::span<const std::string_view> args)
{
    for (int i = 1; i < args.size(); ++i)
    {
        std::string_view arg = args[i];
        if (arg.starts_with("--force-"))
        {
            std::string_view backend = arg.substr(8);
            if (backend == "opengl"sv || backend == "gl"sv || backend == "gles"sv)
                return DriverPreference::OpenGL;
            if (backend == "d3d11"sv)
                return DriverPreference::D3D11;
            if (backend == "d3d12"sv)
                return DriverPreference::D3D12;
            if (backend == "vulkan"sv || backend == "vk"sv)
                return DriverPreference::Vulkan;
            if (backend == "metal"sv || backend == "mtl"sv)
                return DriverPreference::Metal;
        }
    }
    return DriverPreference::Auto;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////////////////////////////

Application* ApplicationCore::getInstance()
{
    AX_ASSERT(s_axmolApp);
    return s_axmolApp;
}

ApplicationCore::ApplicationCore()
{
    s_director = Director::getInstance();
}

ApplicationCore::~ApplicationCore()
{
    Director::destroyInstance();
}

int ApplicationCore::launch(int argc, tchar_t** argv)
{
    CommandLineArgs args;
    args.buildFromArgv(argc, argv);
    auto driverPreference = parseDriverPreference(args.views());
    DriverContext::setDriverPreference(driverPreference);

    return this->run();
}

void ApplicationCore::applicationScreenSizeChanged(int newWidth, int newHeight) {}

void ApplicationCore::setContextAttrs(const ContextAttrs& attrs)
{
    s_contextAttrs = attrs;

    // On macOS, the render scale mode is always physical, so we don't allow changing it to logical.
    // On other platforms, if the user not explicitly sets a render scale mode, we use logical to avoid DPI scaling by
    // default.
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    s_contextAttrs.renderScaleMode = RenderScaleMode::Physical;
#else
    if (attrs.renderScaleMode == RenderScaleMode::Default)
        s_contextAttrs.renderScaleMode = RenderScaleMode::Logical;
#endif
}

}  // namespace ax

// For wasm & android
void _axmolPerformFrameBoundaryTasks()
{
    ax::ApplicationCore::getDirector()->performFrameBoundaryTasks();
}
