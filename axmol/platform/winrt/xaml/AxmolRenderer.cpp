/*
 * cocos2d-x   https://axmol.dev/
 *
 * Copyright (c) 2010-2014 - cocos2d-x community
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/platform/winrt/xaml/AxmolRenderer.h"
#include "AppDelegate.h"
#include "axmol/platform/winrt/RenderViewImpl-winrt.h"
#include "axmol/platform/Application.h"
#include "axmol/renderer/TextureCache.h"

// These are used by the shader compilation methods.
#include <vector>
#include <iostream>
#include <fstream>

using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace ax;

namespace
{
std::unique_ptr<AppDelegate> appDelegate;
}

AxmolRenderer::AxmolRenderer(int width,
                             int height,
                             float dpi,
                             DisplayOrientations orientation,
                             CoreDispatcher const& dispatcher,
                             Panel const& panel)
    : m_width(width), m_height(height), m_dpi(dpi), m_dispatcher(dispatcher), m_panel(panel), m_orientation(orientation)
{
    appDelegate.reset(new AppDelegate());
}

AxmolRenderer::~AxmolRenderer() {}

void AxmolRenderer::Resume()
{
    auto director = ax::Director::getInstance();
    auto glview   = director->getRenderView();

    if (!glview)
    {
        RenderViewImpl* glview = RenderViewImpl::createWithRect(
            "axmol2", ax::Rect{0, 0, static_cast<float>(m_width), static_cast<float>(m_height)});
        glview->UpdateOrientation(m_orientation);
        glview->SetDPI(m_dpi);
        glview->setDispatcher(m_dispatcher);
        glview->setPanel(m_panel);
        director->setRenderView(glview);
        Application::getInstance()->run();
    }
    else
    {
        Application::getInstance()->applicationWillEnterForeground();
        ax::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
    }
}

void AxmolRenderer::Pause()
{
    if (Director::getInstance()->getRenderView())
    {
        Application::getInstance()->applicationDidEnterBackground();
        ax::EventCustom backgroundEvent(EVENT_COME_TO_BACKGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent, true);
    }
}

bool AxmolRenderer::AppShouldExit()
{
    return RenderViewImpl::sharedRenderView()->AppShouldExit();
}

void AxmolRenderer::DeviceLost()
{
    Pause();

    auto director = ax::Director::getInstance();
    if (director->getRenderView())
    {
        axdrv->resetState();
        ax::Director::getInstance()->resetMatrixStack();
        ax::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
        director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);
        director->setGLDefaultValues();
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        ax::VolatileTextureMgr::reloadAllTextures();
#endif

        Application::getInstance()->applicationWillEnterForeground();
        ax::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
    }
}

void AxmolRenderer::SetQueueOperationCb(std::function<void(AsyncOperation, void*)> cb)
{
    RenderViewImpl::sharedRenderView()->SetQueueOperationCb(std::move(cb));
}

void AxmolRenderer::Draw(size_t width, size_t height, float dpi, DisplayOrientations orientation)
{
    auto renderView = RenderViewImpl::sharedRenderView();

    if (orientation != m_orientation)
    {
        m_orientation = orientation;
        renderView->UpdateOrientation(orientation);
    }

    if (width != m_width || height != m_height)
    {
        m_width  = width;
        m_height = height;
        renderView->UpdateForWindowSizeChange(static_cast<float>(width), static_cast<float>(height));
    }

    if (dpi != m_dpi)
    {
        m_dpi = dpi;
        renderView->SetDPI(m_dpi);
    }

    renderView->ProcessEvents();
    renderView->Render();
}

void AxmolRenderer::QueuePointerEvent(ax::PointerEventType type, Windows::UI::Core::PointerEventArgs const& args)
{
    RenderViewImpl::sharedRenderView()->QueuePointerEvent(type, args);
}

void AxmolRenderer::QueueBackButtonEvent()
{
    RenderViewImpl::sharedRenderView()->QueueBackKeyPress();
}

void AxmolRenderer::QueueKeyboardEvent(WinRTKeyboardEventType type, Windows::UI::Core::KeyEventArgs const& args)
{
    RenderViewImpl::sharedRenderView()->QueueWinRTKeyboardEvent(type, args);
}
