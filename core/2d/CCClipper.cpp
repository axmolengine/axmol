/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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
#include "CCClipper.h"


inline Clipper2Lib::Path64 MakeRandomPoly(int width, int height, unsigned vertCnt)
{
    Clipper2Lib::Path64 result;
    result.reserve(vertCnt);
    for (unsigned i = 0; i < vertCnt; ++i)
        result.push_back(Clipper2Lib::Point64(rand() % width, rand() % height));
    return result;
}

inline Clipper2Lib::Path64 MakeStar(const Clipper2Lib::Point64& center, int radius, int points)
{
    if (!(points % 2))
        --points;
    if (points < 5)
        points = 5;
    Clipper2Lib::Path64 tmp = Clipper2Lib::Ellipse<int64_t>(center, radius, radius, points);
    Clipper2Lib::Path64 result;
    result.reserve(points);
    result.push_back(tmp[0]);
    for (int i = points - 1, j = i / 2; j;)
    {
        result.push_back(tmp[j--]);
        result.push_back(tmp[i--]);
    }
    return result;
}

//
//subject.push_back(Clipper2Lib::MakePath("100, 50, 10, 79, 65, 2, 65, 98, 10, 21"));
//clip.push_back(Clipper2Lib::MakePath("98, 63, 4, 68, 77, 8, 52, 100, 19, 12"));
//solution = Clipper2Lib::Intersect(subject, clip, Clipper2Lib::FillRule::NonZero);
//
//auto draw4Clipper = DrawNode::create();
//draw4Clipper->setPosition(200, 200);
//addChild(draw4Clipper);
//
//Vec2 vertices1[] = {{100, 50}, {10, 79}, {65, 2}, {65, 98}, {10, 21}};
//draw4Clipper->drawPoly(vertices1, 5, true, Color4F::YELLOW);
//Vec2 vertices2[] = {{98, 63}, {4, 68}, {77, 8}, {52, 100}, {19, 12}};
//draw4Clipper->drawPoly(vertices2, 5, true, Color4F::ORANGE);
//
//auto drawIntersect = DrawNode::create();
//drawIntersect->setPosition(200, 200);
//addChild(drawIntersect, 5);
//for (size_t i = 0; i < solution.size(); ++i)
//{
//    Vec2* outPoints = nullptr;
//    for (size_t j = 0; j < solution[i].size(); ++j)
//    {
//        if (!outPoints)
//        {
//            outPoints = new Vec2[solution[i].size()];
//        }
//        outPoints[j] = Vec2(solution[i][j].x, solution[i][j].y);
//    }
//    drawIntersect->drawSolidPoly(outPoints, solution[i].size(), Color4B::GREEN);
//}
//
//
//
//subject.clear();
//clip.clear();
//subject.push_back(MakeStar(Clipper2Lib::Point64(225, 225), 220, 9));
//clip.push_back(Clipper2Lib::Ellipse<int64_t>(Clipper2Lib::Point64(225, 225), 150, 150));
//
//auto solution1 = Clipper2Lib::Intersect(subject, clip, Clipper2Lib::FillRule::NonZero);
//auto solution2 = Clipper2Lib::InflatePaths(solution, -10, Clipper2Lib::JoinType::Round, Clipper2Lib::EndType::Polygon);
//
//
//
//for (size_t i = 0; i < solution1.size(); ++i)
//{
//    Vec2* outPoints1 = nullptr;
//    for (size_t j = 0; j < solution1[i].size(); ++j)
//    {
//        if (!outPoints1)
//        {
//            outPoints1 = new Vec2[solution1[i].size()];
//        }
//        outPoints1[j] = Vec2(solution1[i][j].x, solution1[i][j].y);
//    }
//    drawIntersect->drawPoly(outPoints1, solution1[i].size(), true, Color4F::ORANGE);
//}