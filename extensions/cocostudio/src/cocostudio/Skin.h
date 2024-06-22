/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __CCSKIN_H__
#define __CCSKIN_H__

#include "2d/Sprite.h"
#include "renderer/QuadCommand.h"

#include "ArmatureDefine.h"
#include "Bone.h"
#include "CocosStudioExport.h"

namespace cocostudio
{

class CCS_DLL Skin : public ax::Sprite
{
public:
    static Skin* create();
    static Skin* createWithSpriteFrameName(std::string_view pszSpriteFrameName);
    static Skin* create(std::string_view pszFileName);

public:
    /**
     *  @js ctor
     */
    Skin();

    virtual bool initWithSpriteFrameName(std::string_view spriteFrameName) override;
    virtual bool initWithFile(std::string_view filename) override;

    void updateArmatureTransform();
    void updateTransform() override;

    ax::Mat4 getNodeToWorldTransform() const override;
    ax::Mat4 getNodeToWorldTransformAR() const;

    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    /**
     *  @js NA
     *  @lua NA
     */
    virtual void setSkinData(const BaseData& data);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual const BaseData& getSkinData() const;

    virtual void setBone(Bone* bone);
    virtual Bone* getBone() const;

    virtual std::string_view getDisplayName() const { return _displayName; }

protected:
    BaseData _skinData;
    Bone* _bone;
    Armature* _armature;
    ax::Mat4 _skinTransform;
    std::string _displayName;
    ax::QuadCommand _quadCommand;  // quad command
};

}  // namespace cocostudio

#endif /*__CCSKIN_H__*/
