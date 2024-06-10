/****************************************************************************
Copyright (c) 2010      Ricardo Quesada
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

#ifndef __CCCONFIGURATION_H__
#define __CCCONFIGURATION_H__

#include <string>

#include "base/Object.h"
#include "base/Value.h"
#include "3d/Animate3D.h"

/**
 * @addtogroup base
 * @{
 */
NS_AX_BEGIN

class EventCustom;

/** @class Configuration
 * @brief Configuration contains some openGL variables
 * @since v0.99.0
 * @js NA
 */
class AX_DLL Configuration
{
public:
    /** Returns a shared instance of Configuration.
     *
     * @return An autoreleased Configuration object.
     */
    static Configuration* getInstance();

    /** Purge the shared instance of Configuration.
     */
    static void destroyInstance();

public:
    /** Destructor
     * @js NA
     * @lua NA
     */
    virtual ~Configuration();

    /** OpenGL Max texture size.
     *
     * @return The OpenGL Max texture size.
     */
    int getMaxTextureSize() const;

    /** OpenGL Max Modelview Stack Depth.
     *
     * @return The OpenGL Max Modelview Stack Depth.
     */
    int getMaxModelviewStackDepth() const;

    /** Returns the maximum texture units.
     *
     * @return The maximum texture units.
     * @since v2.0.0
     */
    int getMaxTextureUnits() const;

    /** Whether or not the GPU supports NPOT (Non Power Of Two) textures.
     OpenGL ES 2.0 already supports NPOT (iOS).
     *
     * @return Is true if supports NPOT.
     * @since v0.99.2
     */
    bool supportsNPOT() const;

    /** Whether or not PVR Texture Compressed is supported.
     *
     * @return Is true if supports PVR Texture Compressed.
     */
    bool supportsPVRTC() const;

    /** Whether or not ETC1 Texture Compressed is supported.
     *
     *
     * @return Is true if supports ETC1 Texture Compressed.
     */
    bool supportsETC1() const;

    /** Whether or not ETC2 Texture Compressed is supported.
     *
     *
     * @return Is true if supports ETC2 Texture Compressed.
     */
    bool supportsETC2() const;

    /** Whether or not S3TC Texture Compressed is supported.
     *
     * @return Is true if supports S3TC Texture Compressed.
     */
    bool supportsS3TC() const;

    /** Whether or not ATITC Texture Compressed is supported.
     *
     * @return Is true if supports ATITC Texture Compressed.
     */
    bool supportsATITC() const;

    /** Whether or not ASTC Texture Compressed is supported.
     *
     * @return Is true if supports ASTC Texture Compressed.
     */
    bool supportsASTC() const;

    /** Whether or not BGRA8888 textures are supported.
     *
     * @return Is true if supports BGRA8888 textures.
     * @since v0.99.2
     */
    bool supportsBGRA8888() const;

    /** Whether or not glDiscardFramebufferEXT is supported.
     * @return Is true if supports glDiscardFramebufferEXT.
     * @since v0.99.2
     */
    bool supportsDiscardFramebuffer() const;

    /** Whether or not shareable VAOs are supported.
     *
     * @return Is true if supports shareable VAOs.
     * @since v2.0.0
     */
    bool supportsShareableVAO() const;

    /** Whether or not OES_depth24 is supported.
     *
     * @return Is true if supports OES_depth24.
     * @since v2.0.0
     */
    bool supportsOESDepth24() const;

    /** Whether or not OES_Packed_depth_stencil is supported.
     *
     * @return Is true if supports OES_Packed_depth_stencil.
     * @since v2.0.0
     */
    bool supportsOESPackedDepthStencil() const;

    /** Whether or not glMapBuffer() is supported.
     *
     * On Desktop it returns `true`.
     * On Mobile it checks for the extension `GL_OES_mapbuffer`
     *
     * @return Whether or not `glMapBuffer()` is supported.
     * @since v3.13
     */
    bool supportsMapBuffer() const;

    /** Max supported directional lights in a shader, for MeshRenderer.
     *
     * @return Maximum supported directional lights in a shader.
     * @since v3.3
     */
    int getMaxSupportDirLightInShader() const;

    /** Max supported point lights in a shader, for MeshRenderer.
     *
     * @return Maximum supported point lights in a shader.
     * @since v3.3
     */
    int getMaxSupportPointLightInShader() const;

    /** Max supported spot lights in a shader, for MeshRenderer.
     *
     * @return Maximum supported spot lights in a shader.
     * @since v3.3
     */
    int getMaxSupportSpotLightInShader() const;

    /** get 3d animate quality */
    Animate3DQuality getAnimate3DQuality() const;

    /** Initialize method.
     *
     * @return Is true if initialize success.
     */
    bool init();

    /** Returns the value of a given key as a double.
     *
     * @param key A given key.
     * @param defaultValue if not find the value, return the defaultValue.
     * @return
     */
    const Value& getValue(std::string_view key, const Value& defaultValue = Value::Null) const;

    /** Sets a new key/value pair  in the configuration dictionary.
     *
     * @param key A given key.
     * @param value A given value.
     */
    void setValue(std::string_view key, const Value& value);

    /** Returns the Configuration info.
     *
     * @return The Configuration info.
     */
    std::string getInfo() const;

    /** Gathers OpenGL / GPU information.
     */
    void gatherGPUInfo();

    /** Loads a config file. If the keys are already present, then they are going to be replaced. Otherwise the new keys
     * are added.
     *
     * @param filename Config file name.
     */
    void loadConfigFile(std::string_view filename);

    static const char* CONFIG_FILE_LOADED;

    int getMaxAttributes() const;

private:
    Configuration();
    static Configuration* s_sharedConfiguration;
    static std::string s_configfile;

protected:
    int _maxModelviewStackDepth;
    bool _supportsPVRTC;
    bool _supportsETC1;
    bool _supportsETC2;
    bool _supportsS3TC;
    bool _supportsATITC;
    bool _supportsASTC;
    bool _supportsNPOT;
    bool _supportsBGRA8888;
    bool _supportsDiscardFramebuffer;
    bool _supportsShareableVAO;
    bool _supportsOESMapBuffer;
    bool _supportsOESDepth24;
    bool _supportsOESPackedDepthStencil;

    std::string _glExtensions;
    int _maxDirLightInShader;            // max support directional light in shader
    int _maxPointLightInShader;          // max support point light in shader
    int _maxSpotLightInShader;           // max support spot light in shader
    Animate3DQuality _animate3DQuality;  // animate 3d quality

    ValueMap _valueDict;

    EventCustom* _loadedEvent;
};

NS_AX_END
// end of base group
/// @}

#endif  // __CCCONFIGURATION_H__
