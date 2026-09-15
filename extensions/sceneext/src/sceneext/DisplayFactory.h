/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ArmatureDefine.h"
#include "sceneext/AnimationData.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class Skin;
class Bone;
class DecorativeDisplay;
class DisplayData;
/**
 *  @lua NA
 */
class SCNEXT_API DisplayFactory
{
public:
    static void addDisplay(Bone* bone, DecorativeDisplay* decoDisplay, DisplayData* displayData);
    static void createDisplay(Bone* bone, DecorativeDisplay* decoDisplay);
    static void updateDisplay(Bone* bone, float dt, bool dirty);

    static void addSpriteDisplay(Bone* bone, DecorativeDisplay* decoDisplay, DisplayData* displayData);
    static void createSpriteDisplay(Bone* bone, DecorativeDisplay* decoDisplay);
    static void initSpriteDisplay(Bone* bone, DecorativeDisplay* decoDisplay, std::string_view displayName, Skin* skin);

    static void addArmatureDisplay(Bone* bone, DecorativeDisplay* decoDisplay, DisplayData* displayData);
    static void createArmatureDisplay(Bone* bone, DecorativeDisplay* decoDisplay);
    static void updateArmatureDisplay(Bone* bone, ax::Node* display, float dt);

    static void addParticleDisplay(Bone* bone, DecorativeDisplay* decoDisplay, DisplayData* displayData);
    static void createParticleDisplay(Bone* bone, DecorativeDisplay* decoDisplay);
    static void updateParticleDisplay(Bone* bone, ax::Node* display, float dt);
};

}  // namespace ax::ext
