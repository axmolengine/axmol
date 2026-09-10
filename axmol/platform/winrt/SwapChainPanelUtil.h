/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
