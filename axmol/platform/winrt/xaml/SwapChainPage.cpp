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

#include "axmol/platform/winrt/xaml/SwapChainPage.h"
#include "SwapChainPage.g.cpp"
#include "AppDelegate.h"

#include "axmol/platform/Application.h"
#include "axmol/rhi/DriverContext.h"

#include <memory>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace
{
std::unique_ptr<AppDelegate> appDelegate;
}

namespace winrt::AxmolAppWinRT::implementation
{
SwapChainPage::SwapChainPage()
{
    appDelegate.reset(new AppDelegate());
    ax::Application::getInstance()->initContextAttrs();

    // Try to initialize a high-performance graphics driver first.
    // If any of the high-performance APIs (D3D11/D3D12/Vulkan/Metal) are enabled,
    // the runtime will attempt initialization in the default priority order.
    // If all attempts fail, OpenGL will then be explicitly selected as the fallback.
    ax::rhi::DriverContext::makeCurrentDriver();

    InitializeComponent();

    Loaded({this, &SwapChainPage::OnPageLoaded});
    Unloaded({this, &SwapChainPage::OnPageUnloaded});
}

SwapChainPage::~SwapChainPage()
{
    ax::Application::getInstance()->shutdown();
}

void SwapChainPage::OnPageLoaded(IInspectable const&, RoutedEventArgs const&)
{
    m_swapChainPanel = FindName(L"swapChainPanel").as<SwapChainPanel>();
    ax::Application::getInstance()->boot(m_swapChainPanel);
}

void SwapChainPage::OnPageUnloaded(IInspectable const&, RoutedEventArgs const&)
{
    ax::Application::getInstance()->shutdown();
}

}  // namespace winrt::AxmolAppWinRT::implementation
