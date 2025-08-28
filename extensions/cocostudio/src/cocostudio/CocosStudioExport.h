#pragma once

#include "axmol/base/hlookup.h"
#include "axmol/platform/PlatformMacros.h"

#if defined(_WIN32)
#    if defined(CCS_DLLEXPORT)
#        define CCS_API __declspec(dllexport)
#    elif defined(CCS_DLLIMPORT)
#        define CCS_API __declspec(dllimport)
#    endif
#elif defined(CCS_DLLEXPORT) || defined(CCS_DLLIMPORT)
#    define CCS_API __attribute__((visibility("default")))
#endif
#ifndef CCS_API
#    define CCS_API
#endif

#define CCS_DLL CCS_API
