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

#include "SwapChainPage.g.h"

#include <memory>
#include <condition_variable>
#include <mutex>
#include <concrt.h>
#include <concurrent_queue.h>

#include "axmol/platform/winrt/xaml/AxmolRenderer.h"

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.Devices.Input.h>

#if AX_RENDER_API == AX_RENDER_API_GL
#    include "axmol/platform/winrt/xaml/EGLSurfaceProvider.h"
#endif

using namespace winrt;

namespace winrt::AxmolAppWinRT::implementation
{
struct SwapChainPage : SwapChainPageT<SwapChainPage>
{
public:
    SwapChainPage();
    ~SwapChainPage() override;
    void SetVisibility(bool isVisible);

    void ProcessOperations();

    void OnPageLoaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    void OnPanelSizeChanged(Windows::Foundation::IInspectable const& sender,
                            Windows::UI::Xaml::RoutedEventArgs const& e);
    void OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender,
                             Windows::UI::Core::VisibilityChangedEventArgs const& args);
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP) || _MSC_VER >= 1900
    void OnBackButtonPressed(Windows::Foundation::IInspectable const& sender,
                             Windows::UI::Core::BackRequestedEventArgs const& args);
#endif
    void CreateRenderSurface();
    void DestroyRenderSurface();
    void RecoverFromLostDevice();
    void TerminateApp();
    void StartRenderLoop();
    void StopRenderLoop();

    void CreateInput();

#if AX_RENDER_API == AX_RENDER_API_GL
    EGLSurfaceProvider* m_eglSurfaceProvider{nullptr};
    EGLSurface m_eglSurface{EGL_NO_SURFACE};  // This surface is associated with a swapChainPanel on the page
    Concurrency::critical_section m_eglSurfaceCriticalSection{};
#endif
    std::shared_ptr<AxmolRenderer> m_renderer{};

    Windows::Foundation::IAsyncAction m_renderLoopWorker{};

    // Track user input on a background worker thread.
    Windows::Foundation::IAsyncAction m_inputLoopWorker{};
    Windows::UI::Core::CoreIndependentInputSource m_coreInput{nullptr};

    // Independent touch and pen handling functions.
    // !!!Note: cppwinrt generator will Xaml::RoutedEventArgs, so add underline prefix
    void _OnPointerPressed(Windows::Foundation::IInspectable const& sender,
                           Windows::UI::Core::PointerEventArgs const& args);
    void _OnPointerMoved(Windows::Foundation::IInspectable const& sender,
                         Windows::UI::Core::PointerEventArgs const& args);
    void _OnPointerReleased(Windows::Foundation::IInspectable const& sender,
                            Windows::UI::Core::PointerEventArgs const& args);
    void _OnPointerWheelChanged(Windows::Foundation::IInspectable const& sender,
                                Windows::UI::Core::PointerEventArgs const& args);

    // Independent keyboard handling functions.
    void OnKeyPressed(Windows::UI::Core::CoreWindow const& sender, Windows::UI::Core::KeyEventArgs const& args);
    void OnKeyReleased(Windows::UI::Core::CoreWindow const& sender, Windows::UI::Core::KeyEventArgs const& args);

    void _OnCharacterReceived(Windows::UI::Core::CoreWindow const& sender,
                              Windows::UI::Core::CharacterReceivedEventArgs const& args);

    void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation const& sender,
                              Windows::Foundation::IInspectable const& args);

    float m_dpi;
    bool m_deviceLost;
    bool m_visible;
    bool m_cursorVisible;
    Windows::Graphics::Display::DisplayOrientations m_orientation;

    std::mutex m_sleepMutex;

private:
    // must call at UI thread
    void UpdateCanvasSize();

    std::condition_variable m_sleepCondition;

    Concurrency::concurrent_queue<std::function<void()>> m_operations;

    double m_canvasWidth{0};
    double m_canvasHeight{0};
};
}  // namespace winrt::AxmolAppWinRT::implementation

namespace winrt::AxmolAppWinRT::factory_implementation
{
struct SwapChainPage : SwapChainPageT<SwapChainPage, implementation::SwapChainPage>
{};
}  // namespace winrt::AxmolAppWinRT::factory_implementation
