# /****************************************************************************
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# https://axmol.dev/
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

if(ANDROID)
  # refer to target: axmol/platform/android/CMakeLists.txt
  set(_AX_PLATFORM_SPECIFIC_SRC
    platform/android/Application-android.cpp
    platform/android/Common-android.cpp
    platform/android/RenderViewImpl-android.cpp
    platform/android/FileUtils-android.cpp
  )
elseif(WINDOWS)
  if(NOT WINRT)
    set(_AX_PLATFORM_SPECIFIC_HEADER
      platform/win32/Application-win32.h
      platform/win32/GL-win32.h
      platform/win32/FileUtils-win32.h
      platform/win32/StdC-win32.h
    )
    set(_AX_PLATFORM_SPECIFIC_SRC
      platform/win32/StdC-win32.cpp
      platform/win32/FileUtils-win32.cpp
      platform/win32/Common-win32.cpp
      platform/win32/Application-win32.cpp
      platform/win32/Device-win32.cpp
    )
  else()
    file(GLOB _AX_PLATFORM_SPECIFIC_HEADER platform/winrt/*.h)
    file(GLOB _AX_PLATFORM_SPECIFIC_SRC platform/winrt/*.cpp)
  endif()

elseif(APPLE)
  set(_AX_PLATFORM_SPECIFIC_HEADER
    platform/apple/FileUtils-apple.h
    platform/apple/Device-apple.h
  )
  set(_AX_PLATFORM_SPECIFIC_SRC
    platform/apple/FileUtils-apple.mm
    platform/apple/Device-apple.mm
  )

  if(MACOSX)
    set(_AX_PLATFORM_SPECIFIC_HEADER
      ${_AX_PLATFORM_SPECIFIC_HEADER}
      platform/mac/StdC-mac.h
      platform/mac/Application-mac.h
    )
    set(_AX_PLATFORM_SPECIFIC_SRC
      ${_AX_PLATFORM_SPECIFIC_SRC}
      platform/mac/Application-mac.mm
      platform/mac/Common-mac.mm
      platform/mac/Device-mac.mm
    )
    set_source_files_properties(platform/desktop/RenderViewImpl.cpp PROPERTIES LANGUAGE OBJCXX)
  elseif(IOS)
    set(_AX_PLATFORM_SPECIFIC_HEADER
      ${_AX_PLATFORM_SPECIFIC_HEADER}
      platform/ios/Application-ios.h
      platform/ios/DirectorCaller-ios.h
      platform/ios/EARenderView-ios.h
      platform/ios/RenderViewImpl-ios.h
      platform/ios/StdC-ios.h
      platform/ios/InputView-ios.h
    )
    set(_AX_PLATFORM_SPECIFIC_SRC
      ${_AX_PLATFORM_SPECIFIC_SRC}
      platform/ios/Application-ios.mm
      platform/ios/Common-ios.mm
      platform/ios/Device-ios.mm
      platform/ios/DirectorCaller-ios.mm
      platform/ios/EARenderView-ios.mm
      platform/ios/RenderViewImpl-ios.mm
      platform/ios/Image-ios.mm
      platform/ios/InputView-ios.mm
    )

    if(AX_RENDER_API STREQUAL "gl")
      set(_AX_PLATFORM_SPECIFIC_HEADER
        ${_AX_PLATFORM_SPECIFIC_HEADER}
        platform/ios/ES3Renderer-ios.h
        platform/ios/ESRenderer-ios.h
        platform/ios/GL-ios.h
        platform/ios/OpenGL_Internal-ios.h
      )
      set(_AX_PLATFORM_SPECIFIC_SRC
        ${_AX_PLATFORM_SPECIFIC_SRC}
        platform/ios/ES3Renderer-ios.m
      )
    endif()
  endif()

elseif(LINUX)
  if(NOT CMAKE_CROSSCOMPILING)
    include_directories(
      /usr/include
      /usr/include/GLFW
      /usr/local/include/GLFW
    )
  endif(NOT CMAKE_CROSSCOMPILING)

  set(_AX_PLATFORM_SPECIFIC_HEADER
    platform/linux/Application-linux.h
    platform/linux/GL-linux.h
    platform/linux/StdC-linux.h
    platform/linux/FileUtils-linux.h
  )
  set(_AX_PLATFORM_SPECIFIC_SRC
    platform/linux/FileUtils-linux.cpp
    platform/linux/Common-linux.cpp
    platform/linux/Application-linux.cpp
    platform/linux/Device-linux.cpp
  )
elseif(EMSCRIPTEN)
  include_directories(
    PUBLIC "${_AX_ROOT}/3rdparty/angle/_x/include"
    PUBLIC "${_AX_ROOT}/axmol"
  )
  set(_AX_PLATFORM_SPECIFIC_HEADER
    platform/wasm/Application-wasm.h
    platform/wasm/GL-wasm.h
    platform/wasm/StdC-wasm.h
    platform/wasm/FileUtils-wasm.h
  )
  set(_AX_PLATFORM_SPECIFIC_SRC
    platform/wasm/FileUtils-wasm.cpp
    platform/wasm/Common-wasm.cpp
    platform/wasm/Application-wasm.cpp
    platform/wasm/Device-wasm.cpp
  )

  if(AX_WASM_ENABLE_DEVTOOLS)
    list(APPEND _AX_PLATFORM_SPECIFIC_SRC platform/wasm/devtools-wasm.cpp)
  endif()
endif()

if((WIN32 AND NOT WINRT) OR LINUX OR MACOSX OR WASM)
  list(APPEND _AX_PLATFORM_SPECIFIC_HEADER "platform/desktop/RenderViewImpl.h")
  list(APPEND _AX_PLATFORM_SPECIFIC_SRC "platform/desktop/RenderViewImpl.cpp")
endif()

set(_AX_PLATFORM_HEADER
  ${_AX_PLATFORM_SPECIFIC_HEADER}
  platform/Application.h
  platform/ApplicationBase.h
  platform/Common.h
  platform/Device.h
  platform/FileUtils.h
  platform/GL.h
  platform/RenderView.h
  platform/Image.h
  platform/PlatformConfig.h
  platform/PlatformDefine.h
  platform/PlatformMacros.h
  platform/SAXParser.h
  platform/StdC.h
  platform/IFileStream.h
  platform/FileStream.h
)

set(_AX_PLATFORM_SRC
  ${_AX_PLATFORM_SPECIFIC_SRC}
  platform/SAXParser.cpp
  platform/RenderView.cpp
  platform/FileUtils.cpp
  platform/Image.cpp
  platform/FileStream.cpp
  platform/ApplicationBase.cpp
)
