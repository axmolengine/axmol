#pragma once

#include "axmol/tlx/hlookup.hpp"
#include "extensions/ExtensionMacros.h"

/// The sceneext APIs
#if defined(_WIN32)
#    if defined(SCNEXT_EXPORTS)
#        define SCNEXT_API __declspec(dllexport)
#    elif defined(SCNEXT_IMPORTS)
#        define SCNEXT_API __declspec(dllimport)
#    endif
#elif defined(SCNEXT_EXPORTS) || defined(SCNEXT_IMPORTS)
#    define SCNEXT_API __attribute__((visibility("default")))
#endif
#ifndef SCNEXT_API
#    define SCNEXT_API
#endif

/// The sceneio runtimes APIs
#if defined(_WIN32)
#    if defined(SCNIO_EXPORTS)
#        define SCNIO_API __declspec(dllexport)
#    elif defined(SCNIO_IMPORTS)
#        define SCNIO_API __declspec(dllimport)
#    endif
#elif defined(SCNIO_EXPORTS) || defined(SCNIO_IMPORTS)
#    define SCNIO_API __attribute__((visibility("default")))
#endif
#ifndef SCNIO_API
#    define SCNIO_API
#endif
