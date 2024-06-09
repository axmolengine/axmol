/****************************************************************************
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

#include <doctest.h>
#include "2d/Node.h"
#include "base/Map.h"

USING_NS_AX;


TEST_SUITE("base/Map") {
    TEST_CASE("old_tests") {
        auto createMap = []() {
            StringMap<Node*> ret;
            for (int i = 0; i < 20; ++i)
            {
                auto node = Node::create();
                node->setTag(1000 + i);
                ret.insert(StringUtils::toString(i), node);
            }

            return ret;
        };

        // Default constructor
        Map<std::string, Node*> map1;
        CHECK(map1.empty());
        CHECK(map1.size() == 0);
        CHECK(map1.keys().empty());
        CHECK(map1.keys(Node::create()).empty());

        // Move constructor
        auto map2 = createMap();
        for (const auto& e : map2)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 2);
        }

        // Copy constructor
        auto map3(map2);
        for (const auto& e : map3)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 3);
        }

        // Move assignment operator
        StringMap<Node*> map4;
        auto unusedNode = Node::create();
        map4.insert("unused", unusedNode);
        map4 = createMap();
        CHECK(unusedNode->getReferenceCount() == 1);
        for (const auto& e : map4)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 2);
        }

        // Copy assignment operator
        StringMap<Node*> map5;
        map5 = map4;
        for (const auto& e : map5)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 3);
        }

        // Check size
        CHECK(map4.size() == map5.size());

        for (const auto& e : map4)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second == map5.find(e.first)->second);
        }

        // keys and at
        auto keys = map4.keys();
        std::sort(keys.begin(), keys.end());
        CHECK(keys.size() == 20);
        const char* keyValues[20] = {"0", "1", "10", "11", "12", "13", "14", "15", "16", "17",
                            "18", "19", "2", "3", "4", "5", "6", "7", "8", "9"};
        for (int i = 0; i < 20; ++i)
        {
            CHECK(keys[i] == keyValues[i]);
        }

        auto node10Key = map4.at("10");
        map4.insert("100", node10Key);
        map4.insert("101", node10Key);
        map4.insert("102", node10Key);

        auto keysForObject = map4.keys(node10Key);
        std::sort(keysForObject.begin(), keysForObject.end());
        const char* keyValues2[4] = {"10", "100", "101", "102"};
        for (int i = 0; i < 4; ++i)
        {
            CHECK(keysForObject[i] == keyValues2[i]);
        }

        // at in const function
        const auto& constMap = map4;
        CHECK(constMap.at("0")->getTag() == 1000);
        CHECK(constMap.find("1")->second->getTag() == 1001);

        // find
        auto nodeToFind = map4.find("10");
        AX_UNUSED_PARAM(nodeToFind);
        CHECK(nodeToFind->second->getTag() == 1010);

        // insert
        Map<std::string, Node*> map6;
        auto node1 = Node::create();
        node1->setTag(101);
        auto node2 = Node::create();
        node2->setTag(102);
        auto node3 = Node::create();
        node3->setTag(103);
        map6.insert("insert01", node1);
        map6.insert("insert02", node2);
        map6.insert("insert03", node3);

        CHECK(node1->getReferenceCount() == 2);
        CHECK(node2->getReferenceCount() == 2);
        CHECK(node3->getReferenceCount() == 2);
        CHECK(map6.at("insert01") == node1);
        CHECK(map6.at("insert02") == node2);
        CHECK(map6.at("insert03") == node3);

        // erase
        StringMap<Node*> mapForErase = createMap();
        mapForErase.erase(mapForErase.find("9"));
        CHECK(mapForErase.find("9") == mapForErase.end());
        CHECK(mapForErase.size() == 19);

        mapForErase.erase("7");
        CHECK(mapForErase.find("7") == mapForErase.end());
        CHECK(mapForErase.size() == 18);

        std::vector<std::string> itemsToRemove;
        itemsToRemove.emplace_back("2");
        itemsToRemove.emplace_back("3");
        itemsToRemove.emplace_back("4");
        mapForErase.erase(itemsToRemove);
        CHECK(mapForErase.size() == 15);

        // clear
        StringMap<Node*> mapForClear = createMap();
        auto mapForClearCopy         = mapForClear;
        mapForClear.clear();

        for (const auto& e : mapForClearCopy)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 2);
        }

        // get random object
        // Set the seed by time
        std::srand((unsigned)time(nullptr));
        StringMap<Node*> mapForRandom = createMap();
        auto different = 0;
        for (int i = 0; i < mapForRandom.size(); ++i)
        {
            auto a = mapForRandom.getRandomObject();
            auto b = mapForRandom.getRandomObject();
            if (a->getTag() != b->getTag())
                different++;
        }
        CHECK(different > 0);
        CHECK(different > mapForRandom.size() * 3 / 4); // At least 75% different

        // Self assignment
        StringMap<Node*> mapForSelfAssign = createMap();
        mapForSelfAssign                  = mapForSelfAssign;
        CHECK(mapForSelfAssign.size() == 20);

        for (const auto& e : mapForSelfAssign)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 2);
        }

        mapForSelfAssign = std::move(mapForSelfAssign);
        CHECK(mapForSelfAssign.size() == 20);

        for (const auto& e : mapForSelfAssign)
        {
            AX_UNUSED_PARAM(e);
            CHECK(e.second->getReferenceCount() == 2);
        }
    }
}
