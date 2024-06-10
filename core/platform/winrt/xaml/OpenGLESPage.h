/*
* cocos2d-x   https://axmol.dev/
*
* Copyright (c) 2010-2014 - cocos2d-x community
* Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
*
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include "OpenGLESPage.g.h"

#include "OpenGLES.h"
#include <memory>
#include <condition_variable>
#include <mutex>
#include <concrt.h>
#include <concurrent_queue.h>

#include "AxmolRenderer.h"

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.Devices.Input.h>

using namespace winrt;

namespace winrt::AxmolAppWinRT::implementation
{
struct OpenGLESPage : OpenGLESPageT<OpenGLESPage>
    {
    public:
        OpenGLESPage();
        OpenGLESPage(OpenGLES* openGLES);
        virtual ~OpenGLESPage();
        void SetVisibility(bool isVisible);

        void ProcessOperations();

        void OnPageLoaded(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
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

        OpenGLES* mOpenGLES;
        std::shared_ptr<AxmolRenderer> mRenderer;

        EGLSurface mRenderSurface;     // This surface is associated with a swapChainPanel on the page
        Concurrency::critical_section mRenderSurfaceCriticalSection;
        Windows::Foundation::IAsyncAction mRenderLoopWorker;

        // Track user input on a background worker thread.
        Windows::Foundation::IAsyncAction mInputLoopWorker;
        Windows::UI::Core::CoreIndependentInputSource mCoreInput;

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

        float mDpi;
        bool mDeviceLost;
        bool mVisible;
        bool mCursorVisible;
        Windows::Graphics::Display::DisplayOrientations mOrientation;

        std::mutex mSleepMutex;

    public:
        std::condition_variable mSleepCondition;

        Concurrency::concurrent_queue<std::function<void()>> mOperations;
    };
}

namespace winrt::AxmolAppWinRT::factory_implementation
{
struct OpenGLESPage : OpenGLESPageT<OpenGLESPage, implementation::OpenGLESPage>
{};
}  // namespace winrt::AxmolAppWinRT::factory_implementation
