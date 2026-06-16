/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/AutoreleasePool.h"
#include "axmol/base/Types.h"
#include "axmol/platform/Common.h"
#include "axmol/platform/StdC.h"

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

class RenderViewCore;
class Director;
class Application;
class AX_DLL ApplicationCore
{
    friend class RenderViewCore;
    friend class Director;

public:
    /** Since WINDOWS and ANDROID are defined as macros, we could not just use these keywords in enumeration(Platform).
     *  Therefore, we use C# code style to define Platform enums to avoid conflicts with the definitions of system
     * macros.
     */
    enum class Platform
    {
        Unknown,
        Win32,   /**< Win32 Apps */
        WinUWP,  /**< WinUWP */
        Linux,   /**< Linux */
        OSX,     /**< macOS */
        Android, /**< Android */
        iOS,     /**< Apple iOS */
        tvOS,    /**< Apple tvOS */
        Wasm,    /**< aka WebAssembly/Emscripten */
        /* alias */
        Windows    = Win32,
        macOS      = OSX,
        Emscripten = Wasm
    };

    /**
@brief    Get current application instance.
@return Current application instance pointer.
*/
    static Application* getInstance();

    ApplicationCore();

    /**
     * @lua NA
     */
    virtual ~ApplicationCore();

    /**
     * @brief High-level engine launch API.
     *
     * This function serves as the main entry point for starting the engine
     * in a cross-platform environment. It provides advanced initialization
     * by parsing command-line arguments that control engine behavior.
     *
     * Currently, the most important supported option is the Rendering
     * Hardware Interface (RHI) selection, specified via parameters such as
     * `--force-vulkan`, `--force-d3d12`, `--force-metal`, etc.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line arguments, encoded as @c tchar_t strings.
     *             The encoding depends on the platform and build configuration
     *             (UTF-16 on Windows Unicode builds, UTF-8 on other platforms).
     *
     * @return Exit code of the application. Zero indicates successful launch,
     *         non-zero values indicate initialization or runtime errors.
     *
     * @note This API is designed for advanced users and contributors who need
     *       fine-grained control over engine startup. It abstracts away low-level
     *       details while still exposing critical configuration options.
     *
     * @remark Users who prefer a minimal startup without any command-line
     *         argument parsing can directly call @c run(), which bypasses
     *         all advanced parameter handling and executes the engine with
     *         default settings.
     */
    virtual int launch(int argc, tchar_t** argv);

    /**
     * @brief Main execution entry point.
     *
     * Implemented by derived classes to start
     * the application loop and return an exit code.
     *
     * @return Exit code of the application.
     */
    virtual int run() = 0;

    /**
     * @brief    Implement Director and Scene init code here.
     * @return true    Initialize success, app continue.
     * @return false   Initialize failed, app terminate.
     * @lua NA
     */
    virtual bool applicationDidFinishLaunching() = 0;

    /**
     * @brief  This function will be called when the application enters background.
     * @lua NA
     */
    virtual void applicationDidEnterBackground() = 0;

    /**
     * @brief  This function will be called when the application enters foreground.
     * @lua NA
     */
    virtual void applicationWillEnterForeground() = 0;

    /**
     * @brief  This function will be called when the application is about to quit.
     *         Typically triggered when the user clicks the close button or requests
     *         to exit the application.
     * @return void
     * @lua NA
     * @since axmol-2.10.0
     */
    virtual void applicationWillQuit() {};

    /**
     * @brief    Callback by Director for limit FPS.
     * @param interval The time, expressed in seconds, between current frame and next.
     * @lua NA
     */
    virtual void setAnimationInterval(float interval) = 0;

    /** Subclass override the function to set app context attribution instead of use default value.
     * And now can only set six attributions:redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits.
     * Default value are(5,6,5,0,16,0), usually use as follows:
     * void AppDelegate::initContextAttrs(){
     *     ContextAttrs contextAttrs = {8, 8, 8, 8, 24, 8};
     *     setContextAttrs(contextAttrs);
     * }
     */
    virtual void initContextAttrs() {}

    /**
    @brief Get current language config.
    @return Current language config.
    * @lua NA
    */
    virtual LanguageType getCurrentLanguage() = 0;

    /**
     @brief Get current language iso 639-1 code.
     @return Current language iso 639-1 code.
     * @lua NA
     */
    virtual const char* getCurrentLanguageCode() = 0;

    /**
     @brief Get target platform.
     * @lua NA
     */
    virtual Platform getTargetPlatform() = 0;

    /**
     @brief Get application version.
     * @lua NA
     */
    virtual std::string getVersion() = 0;

    /**
     @brief Open url in default browser.
     @param String with url to open.
     @return True if the resource located by the URL was successfully opened; otherwise false.
     * @lua NA
     */
    virtual bool openURL(std::string_view url) = 0;

    static void setContextAttrs(const ContextAttrs& attrs);

    static const ContextAttrs& getContextAttrs() { return s_contextAttrs; }

    static Director* getDirector() { return s_director; }

protected:
    /**
     * @brief Called when the application screen size changes.
     *
     * Users can override this method to listen for screen size changes,
     * including device rotation events. It is recommended to update the
     * designResolutionSize and adjust the layout of objects in the scene
     * accordingly when this callback is triggered.
     *
     * @param newWidth  The new width of the application screen in pixels.
     * @param newHeight The new height of the application screen in pixels.
     */
    virtual void applicationScreenSizeChanged(int newWidth, int newHeight);

    virtual void postBoundaryTaskSignal() {};

    static ContextAttrs s_contextAttrs;
    static Application* s_axmolApp;
    static Director* s_director;
};

// end of platform group
/** @} */

}  // namespace ax
