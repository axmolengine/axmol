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
