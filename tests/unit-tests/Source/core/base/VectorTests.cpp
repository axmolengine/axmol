/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include <doctest.h>
#include "base/Vector.h"

USING_NS_AX;


TEST_SUITE("base/Vector") {
    TEST_CASE("old_tests") {
        Vector<Node*> vec;
        CHECK(vec.empty());
        CHECK(vec.capacity() == 0);
        CHECK(vec.size() == 0);
        CHECK(vec.max_size() > 0);

        auto node1 = Node::create();
        node1->setTag(1);
        vec.pushBack(node1);
        CHECK(node1->getReferenceCount() == 2);

        auto node2 = Node::create();
        node2->setTag(2);
        vec.pushBack(node2);
        CHECK(vec.getIndex(node1) == 0);
        CHECK(vec.getIndex(node2) == 1);

        auto node3 = Node::create();
        node3->setTag(3);
        vec.insert(1, node3);
        CHECK(vec.at(0)->getTag() == 1);
        CHECK(vec.at(1)->getTag() == 3);
        CHECK(vec.at(2)->getTag() == 2);

        // Test copy constructor
        Vector<Node*> vec2(vec);
        CHECK(vec2.size() == vec.size());
        ssize_t size = vec.size();
        for (ssize_t i = 0; i < size; ++i)
        {
            CHECK(vec2.at(i) == vec.at(i));
            CHECK(vec.at(i)->getReferenceCount() == 3);
            CHECK(vec2.at(i)->getReferenceCount() == 3);
        }

        // Test copy assignment operator
        Vector<Node*> vec3;
        vec3 = vec2;
        CHECK(vec3.size() == vec2.size());
        size = vec3.size();
        for (ssize_t i = 0; i < size; ++i)
        {
            CHECK(vec3.at(i) == vec2.at(i));
            CHECK(vec3.at(i)->getReferenceCount() == 4);
            CHECK(vec2.at(i)->getReferenceCount() == 4);
            CHECK(vec.at(i)->getReferenceCount() == 4);
        }

        // Test move constructor

        auto createVector = []() {
            Vector<Node*> ret;

            for (int i = 0; i < 20; i++)
            {
                ret.pushBack(Node::create());
            }

            int j = 1000;
            for (auto&& child : ret)
            {
                child->setTag(j++);
            }

            return ret;
        };

        Vector<Node*> vec4(createVector());
        for (const auto& child : vec4)
        {
            AX_UNUSED_PARAM(child);
            CHECK(child->getReferenceCount() == 2);
        }

        // Test init Vector<T> with capacity
        Vector<Node*> vec5(10);
        CHECK(vec5.capacity() == 10);
        vec5.reserve(20);
        CHECK(vec5.capacity() == 20);

        CHECK(vec5.size() == 0);
        CHECK(vec5.empty());

        auto toRemovedNode = Node::create();
        vec5.pushBack(toRemovedNode);
        CHECK(toRemovedNode->getReferenceCount() == 2);

        // Test move assignment operator
        vec5 = createVector();
        CHECK(toRemovedNode->getReferenceCount() == 1);
        CHECK(vec5.size() == 20);

        for (const auto& child : vec5)
        {
            AX_UNUSED_PARAM(child);
            CHECK(child->getReferenceCount() == 2);
        }

        // Test Vector<T>::find
        CHECK(vec.find(node3) == (vec.begin() + 1));
        CHECK(std::find(std::begin(vec), std::end(vec), node2) == (vec.begin() + 2));

        CHECK(vec.front()->getTag() == 1);
        CHECK(vec.back()->getTag() == 2);

        CHECK(vec.getRandomObject());
        CHECK(!vec.contains(Node::create()));
        CHECK(vec.contains(node1));
        CHECK(vec.contains(node2));
        CHECK(vec.contains(node3));
        CHECK(vec.equals(vec2));
        CHECK(vec.equals(vec3));

        // Insert
        vec5.insert(2, node1);
        CHECK(vec5.at(2)->getTag() == 1);
        CHECK(vec5.size() == 21);
        vec5.back()->setTag(100);
        vec5.popBack();
        CHECK(vec5.size() == 20);
        CHECK(vec5.back()->getTag() != 100);

        // Erase and clear
        Vector<Node*> vec6 = createVector();
        Vector<Node*> vec7 = vec6;  // Copy for check

        CHECK(vec6.size() == 20);
        vec6.erase(vec6.begin() + 1);  //
        CHECK(vec6.size() == 19);
        CHECK((*(vec6.begin() + 1))->getTag() == 1002);
        vec6.erase(vec6.begin() + 2, vec6.begin() + 10);
        CHECK(vec6.size() == 11);
        CHECK(vec6.at(0)->getTag() == 1000);
        CHECK(vec6.at(1)->getTag() == 1002);
        CHECK(vec6.at(2)->getTag() == 1011);
        CHECK(vec6.at(3)->getTag() == 1012);
        vec6.erase(3);
        CHECK(vec6.at(3)->getTag() == 1013);
        vec6.eraseObject(vec6.at(2));
        CHECK(vec6.at(2)->getTag() == 1013);
        vec6.clear();

        auto objA = Node::create();  // retain count is 1
        auto objB = Node::create();
        auto objC = Node::create();
        {
            Vector<Node*> array1;
            Vector<Node*> array2;

            // push back objA 3 times
            array1.pushBack(objA);  // retain count is 2
            array1.pushBack(objA);  // retain count is 3
            array1.pushBack(objA);  // retain count is 4

            array2.pushBack(objA);  // retain count is 5
            array2.pushBack(objB);
            array2.pushBack(objC);

            for (auto&& obj : array1)
            {
                array2.eraseObject(obj);
            }
            CHECK(objA->getReferenceCount() == 4);
        }
        CHECK(objA->getReferenceCount() == 1);

        {
            Vector<Node*> array1;
            // push back objA 3 times
            array1.pushBack(objA);  // retain count is 2
            array1.pushBack(objA);  // retain count is 3
            array1.pushBack(objA);  // retain count is 4
            CHECK(objA->getReferenceCount() == 4);
            array1.eraseObject(objA, true);  // Remove all occurrences in the Vector.
            CHECK(objA->getReferenceCount() == 1);

            array1.pushBack(objA);  // retain count is 2
            array1.pushBack(objA);  // retain count is 3
            array1.pushBack(objA);  // retain count is 4

            array1.eraseObject(objA, false);
            CHECK(objA->getReferenceCount() == 3);  // Only remove the first occurrence in the Vector.
        }

        // Check the retain count in vec7
        CHECK(vec7.size() == 20);
        for (const auto& child : vec7)
        {
            AX_UNUSED_PARAM(child);
            CHECK(child->getReferenceCount() == 2);
        }

        // Sort
        Vector<Node*> vecForSort = createVector();
        std::sort(vecForSort.begin(), vecForSort.end(), [](Node* a, Node* b) { return a->getTag() >= b->getTag(); });

        for (int i = 0; i < 20; ++i)
        {
            CHECK(vecForSort.at(i)->getTag() - 1000 == (19 - i));
        }

        // Reverse
        vecForSort.reverse();
        for (int i = 0; i < 20; ++i)
        {
            CHECK(vecForSort.at(i)->getTag() - 1000 == i);
        }

        // Swap
        Vector<Node*> vecForSwap = createVector();
        vecForSwap.swap(2, 4);
        CHECK(vecForSwap.at(2)->getTag() == 1004);
        CHECK(vecForSwap.at(4)->getTag() == 1002);
        vecForSwap.swap(vecForSwap.at(2), vecForSwap.at(4));
        CHECK(vecForSwap.at(2)->getTag() == 1002);
        CHECK(vecForSwap.at(4)->getTag() == 1004);

        // shrinkToFit
        Vector<Node*> vecForShrink = createVector();
        vecForShrink.reserve(100);
        CHECK(vecForShrink.capacity() == 100);
        vecForShrink.pushBack(Node::create());
        vecForShrink.shrinkToFit();
        CHECK(vecForShrink.capacity() == 21);

        // get random object
        // Set the seed by time
        std::srand((unsigned)time(nullptr));
        Vector<Node*> vecForRandom = createVector();
        auto different = 0;
        for (int i = 0; i < vecForRandom.size(); ++i) {
            auto a = vecForRandom.getRandomObject();
            auto b = vecForRandom.getRandomObject();
            if (a->getTag() != b->getTag())
                different++;
        }
        CHECK(different > 0);
        CHECK(different > vecForRandom.size() * 3 / 4); // At least 75% different

        // Self assignment
        Vector<Node*> vecSelfAssign = createVector();
        vecSelfAssign               = vecSelfAssign;
        CHECK(vecSelfAssign.size() == 20);

        for (const auto& child : vecSelfAssign)
        {
            AX_UNUSED_PARAM(child);
            CHECK(child->getReferenceCount() == 2);
        }

        vecSelfAssign = std::move(vecSelfAssign);
        CHECK(vecSelfAssign.size() == 20);

        for (const auto& child : vecSelfAssign)
        {
            AX_UNUSED_PARAM(child);
            CHECK(child->getReferenceCount() == 2);
        }

        // const at
        const Vector<Node*> vecConstAt = createVector();
        CHECK(vecConstAt.at(8)->getTag() == 1008);
    }
}
