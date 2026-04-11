/****************************************************************************
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

#include <windows.ui.xaml.media.dxinterop.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include "axmol/platform/winrt/DispatcherUtil.h"

namespace ax::winrt
{
using Vector2 = winrt::Windows::Foundation::Numerics::float2;
using winrt::Windows::Foundation::Size;
using winrt::Windows::UI::Xaml::Controls::ISwapChainPanel;
using winrt::Windows::UI::Xaml::Controls::SwapChainPanel;

inline static HRESULT GetSwapChainPanelRenderMetrics(SwapChainPanel& swapChainPanel,
                                                     const CoreDispatcher& dispatcher,
                                                     Size& windowSize,
                                                     Vector2& renderScale)
{
    try
    {
        RunOnUIThreadSync(dispatcher, [&]() {
            windowSize      = swapChainPanel.RenderSize();
            auto panelIface = swapChainPanel.as<ISwapChainPanel>();
            renderScale     = Vector2{panelIface.CompositionScaleX(), panelIface.CompositionScaleY()};
        });
        return S_OK;
    }
    catch (...)
    {
        return E_FAIL;
    }
}
}  // namespace ax::winrt
