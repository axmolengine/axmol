/****************************************************************************
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

#include <doctest.h>
#include <float.h>
#include "2d/Node.h"

using namespace ax;

TEST_SUITE("2d/Node") {
    TEST_CASE("normalized_position") {
        auto parent = Node();
        auto node = Node();

        parent.setContentSize(Vec2(200.0f, 100.0f));
        node.setParent(&parent);
        CHECK_EQ(0.0f, node.getPosition().x);
        CHECK_EQ(0.0f, node.getPosition().y);
        CHECK_EQ(0.0f, node.getNormalizedPosition().x);
        CHECK_EQ(0.0f, node.getNormalizedPosition().y);

        node.setPositionNormalized({0.5f, 0.5f});
        node.visit(nullptr, Mat4::IDENTITY, Node::FLAGS_CONTENT_SIZE_DIRTY);
        CHECK_EQ(100.0f, node.getPosition().x);
        CHECK_EQ(50.0f, node.getPosition().y);
        CHECK_EQ(0.5f, node.getNormalizedPosition().x);
        CHECK_EQ(0.5f, node.getNormalizedPosition().y);

        parent.setContentSize(Vec2(400.0f, 200.0f));
        node.setPosition(100.0f, 50.0f);
        node.visit(nullptr, Mat4::IDENTITY, Node::FLAGS_CONTENT_SIZE_DIRTY);
        CHECK_EQ(100.0f, node.getPosition().x);
        CHECK_EQ(50.0f, node.getPosition().y);

        node.setPosition(0.0f, 0.0f);
        node.setPositionNormalized({0.5f, 0.5f});
        node.visit(nullptr, Mat4::IDENTITY, Node::FLAGS_CONTENT_SIZE_DIRTY);
        CHECK_EQ(200.0f, node.getPosition().x);
        CHECK_EQ(100.0f, node.getPosition().y);
    }
}
