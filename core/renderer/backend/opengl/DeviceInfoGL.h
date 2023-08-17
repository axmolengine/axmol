/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 C4games Ltd

 https://axmolengine.github.io/

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

#include "../DeviceInfo.h"

#include "platform/PlatformConfig.h"
#include "base/hlookup.h"

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _opengl
 * @{
 */

/**
 * Used to query features and implementation limits
 */
class DeviceInfoGL : public DeviceInfo
{
public:
    DeviceInfoGL()          = default;
    virtual ~DeviceInfoGL() = default;

    /**
     * Gather features and implementation limits
     */
    virtual bool init() override;

    /**
     * Get vendor device name.
     * @return Vendor device name.
     */
    virtual const char* getVendor() const override;

    /**
     * Get the full name of the vendor device.
     * @return The full name of the vendor device.
     */
    virtual const char* getRenderer() const override;

    /**
     * Get version name.
     * @return Version name.
     */
    virtual const char* getVersion() const override;

    const char* getShaderVersion() const override;

    /**
     * Check does device has extension.
     */
    virtual bool hasExtension(std::string_view /*extName*/) const override;

    /**
    * Dump all extensions to string
    */
    virtual std::string dumpExtensions() const override;

    /**
     * Check if feature supported by OpenGL ES.
     * @param feature Specify feature to be query.
     * @return true if the feature is supported, false otherwise.
     */
    virtual bool checkForFeatureSupported(FeatureType feature) override;

    /*
    * Check does the device only support GLES2.0
    */
    bool isGLES2Only() const;

private:
    const char* _vendor{nullptr};
    const char* _renderer{nullptr};
    const char* _version{nullptr};
    const char* _shaderVer{nullptr};

    std::set<uint32_t> _glExtensions;

    bool _textureCompressionAstc = false;
    bool _textureCompressionEtc2 = false;
};

// end of _opengl group
/// @}
NS_AX_BACKEND_END
