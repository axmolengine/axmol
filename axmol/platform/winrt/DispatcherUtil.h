/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.h>
#include <exception>

namespace ax::winrt
{
using namespace ::winrt;
using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::UI::Core::CoreDispatcher;
using ::winrt::Windows::UI::Core::CoreDispatcherPriority;
using ::winrt::Windows::UI::Core::DispatchedHandler;

template <typename _Fty>
inline static HRESULT RunOnUIThreadSync(const CoreDispatcher& dispatcher, _Fty&& func)
{
    // Check if already on UI thread
    if (dispatcher.HasThreadAccess())
    {
        // Execute directly
        func();
        return S_OK;
    }

    HRESULT hr = S_OK;
    std::exception_ptr exception;

    // Create DispatcherHandler that captures func
    // Create DispatcherHandler that captures func by move and exception by reference
    DispatchedHandler handler = [func = std::forward<_Fty>(func), &exception]() mutable {
        try
        {
            func();
        }
        catch (...)
        {
            exception = std::current_exception();
        }
    };

    // Schedule execution on UI thread
    IAsyncAction action = dispatcher.RunAsync(CoreDispatcherPriority::Normal, handler);

    // Wait synchronously for completion (throws on timeout or failure)
    // Note: action.get() blocks until async operation completes; calling on UI thread would deadlock,
    // but HasThreadAccess() prevents that.
    try
    {
        action.get();  // Wait for completion
    }
    catch (winrt::hresult_error const& e)
    {
        hr = e.code();  // Convert to HRESULT
    }

    if (exception)
        std::rethrow_exception(exception);

    return hr;
}
}  // namespace ax::winrt
