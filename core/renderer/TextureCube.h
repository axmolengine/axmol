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

#ifndef __CCTEXTURECUBE_H__
#define __CCTEXTURECUBE_H__

#include "renderer/Texture2D.h"
#include "renderer/backend/Texture.h"

#include <string>
#include <unordered_map>
#include "base/Types.h"

NS_AX_BEGIN

/**
 * @addtogroup _3d
 * @{
 */

/**
 TextureCube is a collection of six separate square textures that are put
 onto the faces of an imaginary cube.
*/
class AX_DLL TextureCube : public Object
{
public:
    /** create cube texture from 6 textures.
       @param positive_x texture for the right side of the texture cube face.
       @param negative_x texture for the up side of the texture cube face.
       @param positive_y texture for the top side of the texture cube face
       @param negative_y texture for the bottom side of the texture cube face
       @param positive_z texture for the forward side of the texture cube face.
       @param negative_z texture for the rear side of the texture cube face.
       @return  A new texture cube inited with given parameters.
    */
    static TextureCube* create(std::string_view positive_x,
                               std::string_view negative_x,
                               std::string_view positive_y,
                               std::string_view negative_y,
                               std::string_view positive_z,
                               std::string_view negative_z);

    /** Sets the min filter, mag filter, wrap s and wrap t texture parameters.
    If the texture size is NPOT (non power of 2), then in can only use GL_CLAMP_TO_EDGE in GL_TEXTURE_WRAP_{S,T}.
    */
    void setTexParameters(const Texture2D::TexParams&);

    backend::TextureBackend* getBackendTexture() { return _texture; }

    const std::vector<std::string>& getImagePaths() const { return _imgPath; }

    /** reload texture cube after GLESContext reconstructed.*/
    bool reloadTexture();

    /**
     * Constructor.
     */
    TextureCube();

    /**
     * Destructor.
     */
    virtual ~TextureCube();

protected:
    bool init(std::string_view positive_x,
              std::string_view negative_x,
              std::string_view positive_y,
              std::string_view negative_y,
              std::string_view positive_z,
              std::string_view negative_z);

private:
    std::vector<std::string> _imgPath;
    backend::TextureCubemapBackend* _texture = nullptr;
};

// end of 3d group
/// @}

NS_AX_END

#endif  // __CCTEXTURECUBE_H__
