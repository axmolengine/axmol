/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include <doctest.h>
#include <float.h>
#include "axmol/scene/Node.h"

using namespace ax;

TEST_SUITE("2d/Node")
{
    TEST_CASE("normalized_position")
    {
        auto parent = Node();
        auto node   = Node();

        parent.setContentSize(Vec2(200.0f, 100.0f));
        node.setParent(&parent);
        CHECK_EQ(0.0f, node.getPosition().x);
        CHECK_EQ(0.0f, node.getPosition().y);
        CHECK_EQ(0.0f, node.getNormalizedPosition().x);
        CHECK_EQ(0.0f, node.getNormalizedPosition().y);

        SceneRenderState renderState;

        node.setPositionNormalized({0.5f, 0.5f});
        node.visit(renderState, Mat4::identity, Node::FLAGS_CONTENT_SIZE_DIRTY);
        CHECK_EQ(100.0f, node.getPosition().x);
        CHECK_EQ(50.0f, node.getPosition().y);
        CHECK_EQ(0.5f, node.getNormalizedPosition().x);
        CHECK_EQ(0.5f, node.getNormalizedPosition().y);

        parent.setContentSize(Vec2(400.0f, 200.0f));
        node.setPosition(100.0f, 50.0f);
        node.visit(renderState, Mat4::identity, Node::FLAGS_CONTENT_SIZE_DIRTY);
        CHECK_EQ(100.0f, node.getPosition().x);
        CHECK_EQ(50.0f, node.getPosition().y);

        node.setPosition(0.0f, 0.0f);
        node.setPositionNormalized({0.5f, 0.5f});
        node.visit(renderState, Mat4::identity, Node::FLAGS_CONTENT_SIZE_DIRTY);
        CHECK_EQ(200.0f, node.getPosition().x);
        CHECK_EQ(100.0f, node.getPosition().y);
    }
}
