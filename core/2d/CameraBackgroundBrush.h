/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "base/Types.h"
#include "base/Object.h"
#include "base/EventListenerCustom.h"
#include "3d/Frustum.h"
#include "renderer/QuadCommand.h"
#include "renderer/CustomCommand.h"
#include "renderer/GroupCommand.h"
#include "renderer/backend/Types.h"
#include <vector>

namespace ax
{

class CameraBackgroundColorBrush;
class CameraBackgroundDepthBrush;
class CameraBackgroundSkyBoxBrush;
class Camera;

namespace backend
{
class ProgramState;
class Buffer;
}  // namespace backend

/**
 * Defines a brush to clear the background of camera.
 * There are 4 types of brush. None brush do nothing, Depth brush clear background with given depth, Color brush clear
 * background with given color and depth, Skybox brush clear the background with a skybox. Camera uses depth brush by
 * default.
 */
class AX_DLL CameraBackgroundBrush : public Object
{
public:
    /**
     * Brush types. There are 4 types of brush. See CameraBackgroundDepthBrush, CameraBackgroundColorBrush,
     * CameraBackgroundSkyBoxBrush for more information.
     */
    enum class BrushType
    {
        NONE,    // none brush
        DEPTH,   // depth brush. See CameraBackgroundDepthBrush
        COLOR,   // color brush. See CameraBackgroundColorBrush
        SKYBOX,  // skybox brush. See CameraBackgroundSkyBoxBrush
    };

    /**
     * get brush type
     * @return BrushType
     */
    virtual BrushType getBrushType() const { return BrushType::NONE; }

    /**
     * Creates a none brush, it does nothing when clear the background
     * @return Created brush.
     */
    static CameraBackgroundBrush* createNoneBrush();

    /**
     * Creates a depth brush, which clears depth buffer with a given depth.
     * @param depth Depth used to clear depth buffer
     * @return Created brush
     */
    static CameraBackgroundDepthBrush* createDepthBrush(float depth = 1.f);

    /**
     * Creates a color brush
     * @param color Color of brush
     * @param depth Depth used to clear depth buffer
     * @return Created brush
     */
    static CameraBackgroundColorBrush* createColorBrush(const Color& color, float depth);

    /** Creates a Skybox brush with 6 textures.
     @param positive_x texture for the right side of the texture cube face.
     @param negative_x texture for the up side of the texture cube face.
     @param positive_y texture for the top side of the texture cube face
     @param negative_y texture for the bottom side of the texture cube face
     @param positive_z texture for the forward side of the texture cube face.
     @param negative_z texture for the rear side of the texture cube face.
     @return  A new brush inited with given parameters.
     */
    static CameraBackgroundSkyBoxBrush* createSkyboxBrush(std::string_view positive_x,
                                                          std::string_view negative_x,
                                                          std::string_view positive_y,
                                                          std::string_view negative_y,
                                                          std::string_view positive_z,
                                                          std::string_view negative_z);
    /**
     * draw the background
     */
    virtual void drawBackground(Camera* /*camera*/) {}

    virtual bool isValid() { return true; }

    CameraBackgroundBrush();
    virtual ~CameraBackgroundBrush();

    virtual bool init() { return true; }

protected:
    backend::ProgramState* _programState = nullptr;
};

/**
 * Depth brush clear depth buffer with given depth
 */
class AX_DLL CameraBackgroundDepthBrush : public CameraBackgroundBrush
{
public:
    /**
     * Create a depth brush
     * @param depth Depth used to clear the depth buffer
     * @return Created brush
     */
    static CameraBackgroundDepthBrush* create(float depth);

    /**
     * Get brush type. Should be BrushType::DEPTH
     * @return brush type
     */
    virtual BrushType getBrushType() const override { return BrushType::DEPTH; }

    /**
     * Draw background
     */
    virtual void drawBackground(Camera* camera) override;

    /**
     * Set depth
     * @param depth Depth used to clear depth buffer
     */
    void setDepth(float depth) { _depth = depth; }

    CameraBackgroundDepthBrush();
    virtual ~CameraBackgroundDepthBrush();

    virtual bool init() override;

private:
    void onBeforeDraw();
    void onAfterDraw();

protected:
#if AX_ENABLE_CACHE_TEXTURE_DATA
    EventListenerCustom* _backToForegroundListener;
#endif
    void initBuffer();

protected:
    float _depth;
    backend::UniformLocation _locDepth;
    CustomCommand _customCommand;

    bool _clearColor;
    std::vector<V3F_T2F_C4F> _vertices;
    struct
    {
        uint32_t stencilWriteMask            = 0;
        bool depthTest                       = true;
        backend::CompareFunction compareFunc = backend::CompareFunction::ALWAYS;
    } _stateBlock;
};

/**
 * Color brush clear buffer with given depth and color
 */
class AX_DLL CameraBackgroundColorBrush : public CameraBackgroundDepthBrush
{
public:
    /**
     * Get brush type. Should be BrushType::COLOR
     * @return brush type
     */
    virtual BrushType getBrushType() const override { return BrushType::COLOR; }

    /**
     * Create a color brush
     * @param color Color used to clear the color buffer
     * @param depth Depth used to clear the depth buffer
     * @return Created brush
     */
    static CameraBackgroundColorBrush* create(const Color& color, float depth);

    /**
     * Draw background
     */
    virtual void drawBackground(Camera* camera) override;

    /**
     * Set clear color
     * @param color Color used to clear the color buffer
     */
    void setColor(const Color& color);

    CameraBackgroundColorBrush();
    virtual ~CameraBackgroundColorBrush();

    virtual bool init() override;

protected:
    Color _color;
};

class TextureCube;
class GLProgramState;
class EventListenerCustom;

/**
 * Skybox brush clear buffer with a skybox
 */
class AX_DLL CameraBackgroundSkyBoxBrush : public CameraBackgroundBrush
{
public:
    /**
     * Get brush type. Should be BrushType::SKYBOX
     * @return brush type
     */
    virtual BrushType getBrushType() const override { return BrushType::SKYBOX; }

    /** Creates a Skybox brush with 6 textures.
     @param positive_x texture for the right side of the texture cube face.
     @param negative_x texture for the up side of the texture cube face.
     @param positive_y texture for the top side of the texture cube face
     @param negative_y texture for the bottom side of the texture cube face
     @param positive_z texture for the forward side of the texture cube face.
     @param negative_z texture for the rear side of the texture cube face.
     @return  A new brush inited with given parameters.
     */
    static CameraBackgroundSkyBoxBrush* create(std::string_view positive_x,
                                               std::string_view negative_x,
                                               std::string_view positive_y,
                                               std::string_view negative_y,
                                               std::string_view positive_z,
                                               std::string_view negative_z);

    /** Creates a Skybox brush with 6 textures.
     */
    static CameraBackgroundSkyBoxBrush* create();
    /**
     * Set skybox texture
     * @param texture Skybox texture
     */
    void setTexture(TextureCube* texture);

    TextureCube* getTexture() const { return _texture; }

    /**
     * Draw background
     */
    virtual void drawBackground(Camera* camera) override;

    bool isActived() const;
    void setActived(bool actived);
    virtual void setTextureValid(bool valid);
    virtual bool isValid() override;

    CameraBackgroundSkyBoxBrush();
    virtual ~CameraBackgroundSkyBoxBrush();

    /**
     * init Skybox.
     */
    virtual bool init() override;

private:
    void onBeforeDraw();
    void onAfterDraw();

protected:
    void initBuffer();

    TextureCube* _texture;

#if AX_ENABLE_CACHE_TEXTURE_DATA
    EventListenerCustom* _backToForegroundListener;
#endif

private:
    bool _actived;
    bool _textureValid;

    CustomCommand _customCommand;

    backend::UniformLocation _uniformColorLoc;
    backend::UniformLocation _uniformCameraRotLoc;
    backend::UniformLocation _uniformEnvLoc;

    struct
    {
        bool depthTest                     = true;
        bool depthWrite                    = true;
        backend::CompareFunction depthFunc = backend::CompareFunction::ALWAYS;
        backend::CullMode cullMode         = backend::CullMode::BACK;
    } _stateBlock;
};

}
