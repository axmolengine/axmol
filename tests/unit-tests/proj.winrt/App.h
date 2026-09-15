/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "pch.h"

#include "App.xaml.g.h"

using namespace winrt;

namespace winrt::AxmolAppWinRT::implementation
{
struct App : AppT<App>
{
    App();
    void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
    void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
    void OnResuming(IInspectable const&, IInspectable const&);
    void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);
    Windows::Foundation::IInspectable mPage;
};
}  // namespace winrt::AxmolAppWinRT::implementation
