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

#include <winrt/Windows.UI.Xaml.Markup.h>

#include "SwapChainPage.g.h"

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt::AxmolAppWinRT::implementation
{
struct SwapChainPage : SwapChainPageT<SwapChainPage>
{
    SwapChainPage();
    ~SwapChainPage() override;

    void OnPageLoaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    void OnPageUnloaded(Windows::Foundation::IInspectable const& sender,
                        Windows::UI::Xaml::RoutedEventArgs const& args);

private:
    Windows::UI::Xaml::Controls::SwapChainPanel m_swapChainPanel{nullptr};
};
}  // namespace winrt::AxmolAppWinRT::implementation

namespace winrt::AxmolAppWinRT::factory_implementation
{
struct SwapChainPage : SwapChainPageT<SwapChainPage, implementation::SwapChainPage>
{};
}  // namespace winrt::AxmolAppWinRT::factory_implementation
