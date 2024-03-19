//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2024 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifdef _WIN32

#  include "yasio/config.hpp"
#  include "yasio/utils.hpp"

#  if defined(YASIO__USE_TIMEAPI)
#    include <timeapi.h>
#    pragma comment(lib, "Winmm.lib")
#  endif

namespace yasio
{
/**
* CLASS wtimer_hres: The windows timer high-resolution setup helper
*/
struct wtimer_hres {
  using NTSTATUS = LONG;
  /**
   * @param res the timer resolution in 100-ns units: 100-ns * 10000 = 1ms
   * @remark
   *   - timeapi timeBeginPeriod max resolution is 1ms
   *   - ZwSetTimerResolution max resolution is 0.5ms aka 100-ns * 5000
   */
  static const ULONG _1ms_den = 10000;
  wtimer_hres(ULONG timer_res = _1ms_den)
  {
#  if defined(YASIO__USE_TIMEAPI)
    TIMECAPS tc;
    if (TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(TIMECAPS)))
    {
      timer_res_ = yasio::clamp(static_cast<UINT>(timer_res / _1ms_den), tc.wPeriodMin, tc.wPeriodMax);
      timeBeginPeriod(timer_res_);
    }
#  else
    do
    {
      HMODULE hNtDll = GetModuleHandleW(L"ntdll");
      if (!hNtDll)
        break;

      NTSTATUS(__stdcall * NtQueryTimerResolution)
      (OUT PULONG MinimumResolution, OUT PULONG MaximumResolution, OUT PULONG CurrentResolution) =
          reinterpret_cast<NTSTATUS(__stdcall*)(PULONG, PULONG, PULONG)>(GetProcAddress(hNtDll, "NtQueryTimerResolution"));
      if (!NtQueryTimerResolution)
        break;
      ZwSetTimerResolution = reinterpret_cast<NTSTATUS(__stdcall*)(ULONG, BOOLEAN, PULONG)>(GetProcAddress(hNtDll, "ZwSetTimerResolution"));
      if (!ZwSetTimerResolution)
        break;

      ULONG MinimumResolution, MaximumResolution, CurrentResolution;
      if (NtQueryTimerResolution(&MinimumResolution, &MaximumResolution, &CurrentResolution) != 0)
        break;
      ZwSetTimerResolution(yasio::clamp(timer_res, MaximumResolution, MinimumResolution), TRUE, &timer_res);
    } while (false);
#  endif
  }
  ~wtimer_hres()
  {
#  if defined(YASIO__USE_TIMEAPI)
    if (timer_res_ != 0)
      timeEndPeriod(timer_res_);
#  else
    if (timer_res_)
      ZwSetTimerResolution(timer_res_, FALSE, nullptr);
#  endif
  }
#  if defined(YASIO__USE_TIMEAPI)
  UINT timer_res_{0};
#  else
  NTSTATUS(__stdcall* ZwSetTimerResolution)
  (IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution){nullptr};
  ULONG timer_res_{0};
#  endif
};

} // namespace yasio
#endif
