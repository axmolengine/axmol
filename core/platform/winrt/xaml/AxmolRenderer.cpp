/*
 * cocos2d-x   https://axmolengine.github.io/
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

#include "AxmolRenderer.h"
#include "AppDelegate.h"
#include "platform/winrt/GLViewImpl-winrt.h"
#include "platform/Application.h"
#include "renderer/TextureCache.h"

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
    auto glview   = director->getGLView();

    if (!glview)
    {
        GLViewImpl* glview = GLViewImpl::createWithRect(
            "axmol2", ax::Rect{0, 0, static_cast<float>(m_width), static_cast<float>(m_height)});
        glview->UpdateOrientation(m_orientation);
        glview->SetDPI(m_dpi);
        glview->setDispatcher(m_dispatcher);
        glview->setPanel(m_panel);
        director->setGLView(glview);
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
    if (Director::getInstance()->getGLView())
    {
        Application::getInstance()->applicationDidEnterBackground();
        ax::EventCustom backgroundEvent(EVENT_COME_TO_BACKGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent, true);
    }
}

bool AxmolRenderer::AppShouldExit()
{
    return GLViewImpl::sharedGLView()->AppShouldExit();
}

void AxmolRenderer::DeviceLost()
{
    Pause();

    auto director = ax::Director::getInstance();
    if (director->getGLView())
    {
        // TODO:
        // ax::GL::invalidateStateCache();
        // ax::GLProgramCache::getInstance()->reloadDefaultGLPrograms();
        // ax::DrawPrimitives::init();
        // ax::VolatileTextureMgr::reloadAllTextures();

        ax::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
        director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);
        director->setGLDefaultValues();

        Application::getInstance()->applicationWillEnterForeground();
        ax::EventCustom foregroundEvent(EVENT_COME_TO_FOREGROUND);
        ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
    }
}

void AxmolRenderer::SetQueueOperationCb(std::function<void(AsyncOperation, void*)> cb)
{
    GLViewImpl::sharedGLView()->SetQueueOperationCb(std::move(cb));
}

void AxmolRenderer::Draw(size_t width, size_t height, float dpi, DisplayOrientations orientation)
{
    auto glView = GLViewImpl::sharedGLView();

    if (orientation != m_orientation)
    {
        m_orientation = orientation;
        glView->UpdateOrientation(orientation);
    }

    if (width != m_width || height != m_height)
    {
        m_width  = width;
        m_height = height;
        glView->UpdateForWindowSizeChange(static_cast<float>(width), static_cast<float>(height));
    }

    if (dpi != m_dpi)
    {
        m_dpi = dpi;
        glView->SetDPI(m_dpi);
    }

    glView->ProcessEvents();
    glView->Render();
}

void AxmolRenderer::QueuePointerEvent(ax::PointerEventType type, Windows::UI::Core::PointerEventArgs const& args)
{
    GLViewImpl::sharedGLView()->QueuePointerEvent(type, args);
}

void AxmolRenderer::QueueBackButtonEvent()
{
    GLViewImpl::sharedGLView()->QueueBackKeyPress();
}

void AxmolRenderer::QueueKeyboardEvent(WinRTKeyboardEventType type, Windows::UI::Core::KeyEventArgs const& args)
{
    GLViewImpl::sharedGLView()->QueueWinRTKeyboardEvent(type, args);
}
