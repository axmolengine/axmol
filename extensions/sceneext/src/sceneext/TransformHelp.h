/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ArmatureDefine.h"
#include "sceneext/AnimationData.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

/*
 * use to calculate the matrix of node from parent node
 * @lua NA
 */
class SCNEXT_API TransformHelp
{
public:
    static void transformFromParent(BaseData& node, const BaseData& parentNode);
    static void transformToParent(BaseData& node, const BaseData& parentNode);

    static void transformFromParentWithoutScale(BaseData& node, const BaseData& parentNode);
    static void transformToParentWithoutScale(BaseData& node, const BaseData& parentNode);

    static void nodeToMatrix(const BaseData& _node, ax::AffineTransform& _matrix);
    static void nodeToMatrix(const BaseData& node, ax::Mat4& matrix);
    static void matrixToNode(const ax::AffineTransform& _matrix, BaseData& _node);
    static void matrixToNode(const ax::Mat4& _matrix, BaseData& _node);

    static void nodeConcat(BaseData& target, BaseData& source);
    static void nodeSub(BaseData& target, BaseData& source);

public:
    static ax::AffineTransform helpMatrix1;
    static ax::AffineTransform helpMatrix2;

    static ax::Vec2 helpPoint1;
    static ax::Vec2 helpPoint2;
};

}  // namespace ax::ext
