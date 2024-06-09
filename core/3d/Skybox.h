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

#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "base/Types.h"
#include "platform/PlatformMacros.h"
#include "renderer/CustomCommand.h"
#include "2d/Node.h"
#include "renderer/backend/ProgramState.h"

NS_AX_BEGIN

/**
 * @addtogroup _3d
 * @{
 */

class TextureCube;

/**
 * Sky box technology is usually used to simulate infinity sky, mountains and other phenomena.
 */
class AX_DLL Skybox : public Node
{
public:
    CREATE_FUNC(Skybox);

    /** create skybox from 6 textures.
     @param positive_x texture for the right side of the texture cube face.
     @param negative_x texture for the left side of the texture cube face.
     @param positive_y texture for the top side of the texture cube face
     @param negative_y texture for the bottom side of the texture cube face
     @param positive_z texture for the forward side of the texture cube face.
     @param negative_z texture for the rear side of the texture cube face.
     @return  A new skybox inited with given parameters.
     */
    static Skybox* create(std::string_view positive_x,
                          std::string_view negative_x,
                          std::string_view positive_y,
                          std::string_view negative_y,
                          std::string_view positive_z,
                          std::string_view negative_z);

    /**texture getter and setter*/
    void setTexture(TextureCube*);

    /** draw Skybox object */
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;

    /** reload sky box after GLESContext reconstructed.*/
    void reload();

    /**
     * Constructor.
     */
    Skybox();

    /**
     * Destructor.
     */
    virtual ~Skybox();

    /**
     * init Skybox.
     */
    virtual bool init() override;

    /**
     * initialize with texture path
     */
    bool init(std::string_view positive_x,
              std::string_view negative_x,
              std::string_view positive_y,
              std::string_view negative_y,
              std::string_view positive_z,
              std::string_view negative_z);

protected:
    /**
     * init internal buffers for Skybox.
     */
    void initBuffers();

    CustomCommand _customCommand;
    TextureCube* _texture;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(Skybox);

    backend::UniformLocation _uniformColorLoc;
    backend::UniformLocation _uniformCameraRotLoc;
    backend::UniformLocation _uniformEnvLoc;

    void onBeforeDraw();
    void onAfterDraw();

    bool _rendererDepthTestEnabled;
    backend::CompareFunction _rendererDepthCmpFunc;
    backend::CullMode _rendererCullMode;
};

// end of 3d group
/// @}

NS_AX_END

#endif  // __SKYBOX_H__
