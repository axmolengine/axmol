/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include "2d/AutoPolygon.h"
#include "poly2tri/poly2tri.h"
#include "base/Director.h"
#include "base/axstd.h"
#include "renderer/TextureCache.h"
#include "clipper2/clipper.h"
#include <algorithm>
#include <math.h>

static unsigned short quadIndices9[] = {
    0 + 4 * 0, 1 + 4 * 0, 2 + 4 * 0, 3 + 4 * 0, 2 + 4 * 0, 1 + 4 * 0, 0 + 4 * 1, 1 + 4 * 1, 2 + 4 * 1,
    3 + 4 * 1, 2 + 4 * 1, 1 + 4 * 1, 0 + 4 * 2, 1 + 4 * 2, 2 + 4 * 2, 3 + 4 * 2, 2 + 4 * 2, 1 + 4 * 2,
    0 + 4 * 3, 1 + 4 * 3, 2 + 4 * 3, 3 + 4 * 3, 2 + 4 * 3, 1 + 4 * 3, 0 + 4 * 4, 1 + 4 * 4, 2 + 4 * 4,
    3 + 4 * 4, 2 + 4 * 4, 1 + 4 * 4, 0 + 4 * 5, 1 + 4 * 5, 2 + 4 * 5, 3 + 4 * 5, 2 + 4 * 5, 1 + 4 * 5,
    0 + 4 * 6, 1 + 4 * 6, 2 + 4 * 6, 3 + 4 * 6, 2 + 4 * 6, 1 + 4 * 6, 0 + 4 * 7, 1 + 4 * 7, 2 + 4 * 7,
    3 + 4 * 7, 2 + 4 * 7, 1 + 4 * 7, 0 + 4 * 8, 1 + 4 * 8, 2 + 4 * 8, 3 + 4 * 8, 2 + 4 * 8, 1 + 4 * 8,
};

const static float PRECISION = 10.0f;

NS_AX_BEGIN

PolygonInfo::PolygonInfo() : _isVertsOwner(true), _rect(Rect::ZERO), _filename("")
{
    triangles.verts      = nullptr;
    triangles.indices    = nullptr;
    triangles.vertCount  = 0;
    triangles.indexCount = 0;
};

PolygonInfo::PolygonInfo(const PolygonInfo& other) : triangles(), _isVertsOwner(true), _rect()
{
    _filename         = other._filename;
    _isVertsOwner     = true;
    _rect             = other._rect;
    triangles.verts   = new V3F_C4B_T2F[other.triangles.vertCount];
    triangles.indices = new unsigned short[other.triangles.indexCount];
    AXASSERT(triangles.verts && triangles.indices, "not enough memory");
    triangles.vertCount  = other.triangles.vertCount;
    triangles.indexCount = other.triangles.indexCount;
    memcpy(triangles.verts, other.triangles.verts, other.triangles.vertCount * sizeof(other.triangles.verts[0]));
    memcpy(triangles.indices, other.triangles.indices, other.triangles.indexCount * sizeof(other.triangles.indices[0]));
};

PolygonInfo& PolygonInfo::operator=(const PolygonInfo& other)
{
    if (this != &other)
    {
        releaseVertsAndIndices();
        _filename         = other._filename;
        _isVertsOwner     = true;
        _rect             = other._rect;
        triangles.verts   = new V3F_C4B_T2F[other.triangles.vertCount];
        triangles.indices = new unsigned short[other.triangles.indexCount];
        AXASSERT(triangles.verts && triangles.indices, "not enough memory");
        triangles.vertCount  = other.triangles.vertCount;
        triangles.indexCount = other.triangles.indexCount;
        memcpy(triangles.verts, other.triangles.verts, other.triangles.vertCount * sizeof(other.triangles.verts[0]));
        memcpy(triangles.indices, other.triangles.indices,
               other.triangles.indexCount * sizeof(other.triangles.indices[0]));
    }
    return *this;
}

PolygonInfo::~PolygonInfo()
{
    releaseVertsAndIndices();
}

void PolygonInfo::setQuad(V3F_C4B_T2F_Quad* quad)
{
    releaseVertsAndIndices();
    _isVertsOwner        = false;
    triangles.indices    = quadIndices9;
    triangles.vertCount  = 4;
    triangles.indexCount = 6;
    triangles.verts      = (V3F_C4B_T2F*)quad;
}

void PolygonInfo::setQuads(V3F_C4B_T2F_Quad* quad, int numberOfQuads)
{
    AXASSERT(numberOfQuads >= 1 && numberOfQuads <= 9, "Invalid number of Quads");

    releaseVertsAndIndices();
    _isVertsOwner        = false;
    triangles.indices    = quadIndices9;
    triangles.vertCount  = 4 * numberOfQuads;
    triangles.indexCount = 6 * numberOfQuads;
    triangles.verts      = (V3F_C4B_T2F*)quad;
}

void PolygonInfo::setTriangles(const TrianglesCommand::Triangles& other)
{
    this->releaseVertsAndIndices();
    _isVertsOwner = false;

    this->triangles.vertCount  = other.vertCount;
    this->triangles.indexCount = other.indexCount;
    this->triangles.verts      = other.verts;
    this->triangles.indices    = other.indices;
}

void PolygonInfo::releaseVertsAndIndices()
{
    if (_isVertsOwner)
    {
        if (nullptr != triangles.verts)
        {
            AX_SAFE_DELETE_ARRAY(triangles.verts);
        }

        if (nullptr != triangles.indices)
        {
            AX_SAFE_DELETE_ARRAY(triangles.indices);
        }
    }
}

unsigned int PolygonInfo::getVertCount() const
{
    return (unsigned int)triangles.vertCount;
}

unsigned int PolygonInfo::getTrianglesCount() const
{
    return (unsigned int)triangles.indexCount / 3;
}

float PolygonInfo::getArea() const
{
    float area              = 0;
    V3F_C4B_T2F* verts      = triangles.verts;
    unsigned short* indices = triangles.indices;
    for (unsigned int i = 0; i < triangles.indexCount; i += 3)
    {
        auto A = verts[indices[i]].vertices;
        auto B = verts[indices[i + 1]].vertices;
        auto C = verts[indices[i + 2]].vertices;
        area += (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2;
    }
    return area;
}

AutoPolygon::AutoPolygon(std::string_view filename)
    : _image(nullptr), _data(nullptr), _filename(""), _width(0), _height(0), _scaleFactor(0)
{
    _filename = filename;
    _image    = new Image();
    _image->initWithImageFile(filename);
    AXASSERT(_image->getPixelFormat() == backend::PixelFormat::RGBA8,
             "unsupported format, currently only supports rgba8888");
    _data        = _image->getData();
    _width       = _image->getWidth();
    _height      = _image->getHeight();
    _scaleFactor = Director::getInstance()->getContentScaleFactor();
}

AutoPolygon::~AutoPolygon()
{
    AX_SAFE_DELETE(_image);
}

std::vector<Vec2> AutoPolygon::trace(const Rect& rect, float threshold)
{
    Vec2 first = findFirstNoneTransparentPixel(rect, threshold);
    return marchSquare(rect, first, threshold);
}

Vec2 AutoPolygon::findFirstNoneTransparentPixel(const Rect& rect, float threshold)
{
    bool found = false;
    Vec2 i;
    for (i.y = rect.origin.y; i.y < rect.origin.y + rect.size.height; i.y++)
    {
        if (found)
            break;
        for (i.x = rect.origin.x; i.x < rect.origin.x + rect.size.width; i.x++)
        {
            auto alpha = getAlphaByPos(i);
            if (alpha > threshold)
            {
                found = true;
                break;
            }
        }
    }
    AXASSERT(found, "image is all transparent!");
    return i;
}

unsigned char AutoPolygon::getAlphaByIndex(unsigned int i)
{
    return *(_data + i * 4 + 3);
}
unsigned char AutoPolygon::getAlphaByPos(const Vec2& pos)
{
    return *(_data + ((int)pos.y * _width + (int)pos.x) * 4 + 3);
}

unsigned int AutoPolygon::getSquareValue(unsigned int x, unsigned int y, const Rect& rect, float threshold)
{
    /*
     checking the 2x2 pixel grid, assigning these values to each pixel, if not transparent
     +---+---+
     | 1 | 2 |
     +---+---+
     | 4 | 8 | <- current pixel (curx,cury)
     +---+---+
     */
    unsigned int sv = 0;
    // NOTE: due to the way we pick points from texture, rect needs to be smaller, otherwise it goes outside 1 pixel
    auto fixedRect = Rect(rect.origin, rect.size - Vec2(2, 2));

    Vec2 tl = Vec2(x - 1.0f, y - 1.0f);
    sv += (fixedRect.containsPoint(tl) && getAlphaByPos(tl) > threshold) ? 1 : 0;
    Vec2 tr = Vec2(x - 0.0f, y - 1.0f);
    sv += (fixedRect.containsPoint(tr) && getAlphaByPos(tr) > threshold) ? 2 : 0;
    Vec2 bl = Vec2(x - 1.0f, y - 0.0f);
    sv += (fixedRect.containsPoint(bl) && getAlphaByPos(bl) > threshold) ? 4 : 0;
    Vec2 br = Vec2(x - 0.0f, y - 0.0f);
    sv += (fixedRect.containsPoint(br) && getAlphaByPos(br) > threshold) ? 8 : 0;
    AXASSERT(sv != 0 && sv != 15, "square value should not be 0, or 15");
    return sv;
}

std::vector<ax::Vec2> AutoPolygon::marchSquare(const Rect& rect, const Vec2& start, float threshold)
{
    int stepx          = 0;
    int stepy          = 0;
    int prevx          = 0;
    int prevy          = 0;
    int startx         = (int)start.x;
    int starty         = (int)start.y;
    int curx           = startx;
    int cury           = starty;
    unsigned int count = 0;
    std::vector<int> case9s;
    std::vector<int> case6s;
    int i;
    std::vector<int>::iterator it;
    std::vector<ax::Vec2> _points;
    do
    {
        int sv = getSquareValue(curx, cury, rect, threshold);
        switch (sv)
        {

        case 1:
        case 5:
        case 13:
            /* going UP with these cases:
             1          5           13
             +---+---+  +---+---+  +---+---+
             | 1 |   |  | 1 |   |  | 1 |   |
             +---+---+  +---+---+  +---+---+
             |   |   |  | 4 |   |  | 4 | 8 |
             +---+---+  +---+---+  +---+---+
             */
            stepx = 0;
            stepy = -1;
            break;

        case 8:
        case 10:
        case 11:
            /* going DOWN with these cases:
             8          10          11
             +---+---+  +---+---+   +---+---+
             |   |   |  |   | 2 |   | 1 | 2 |
             +---+---+  +---+---+   +---+---+
             |   | 8 |  |   | 8 |   |   | 8 |
             +---+---+  +---+---+  	+---+---+
             */
            stepx = 0;
            stepy = 1;
            break;

        case 4:
        case 12:
        case 14:
            /* going LEFT with these cases:
             4          12          14
             +---+---+  +---+---+   +---+---+
             |   |   |  |   |   |   |   | 2 |
             +---+---+  +---+---+   +---+---+
             | 4 |   |  | 4 | 8 |   | 4 | 8 |
             +---+---+  +---+---+  	+---+---+
             */
            stepx = -1;
            stepy = 0;
            break;

        case 2:
        case 3:
        case 7:
            /* going RIGHT with these cases:
             2          3           7
             +---+---+  +---+---+   +---+---+
             |   | 2 |  | 1 | 2 |   | 1 | 2 |
             +---+---+  +---+---+   +---+---+
             |   |   |  |   |   |   | 4 |   |
             +---+---+  +---+---+  	+---+---+
             */
            stepx = 1;
            stepy = 0;
            break;
        case 9:
            /*
             +---+---+
             | 1 |   |
             +---+---+
             |   | 8 |
             +---+---+
             this should normally go UP, but if we already been here, we go down
            */
            // find index from xy;
            i  = getIndexFromPos(curx, cury);
            it = find(case9s.begin(), case9s.end(), i);
            if (it != case9s.end())
            {
                // found, so we go down, and delete from case9s;
                stepx = 0;
                stepy = 1;
                case9s.erase(it);
            }
            else
            {
                // not found, we go up, and add to case9s;
                stepx = 0;
                stepy = -1;
                case9s.emplace_back(i);
            }
            break;
        case 6:
            /*
             6
             +---+---+
             |   | 2 |
             +---+---+
             | 4 |   |
             +---+---+
             this normally go RIGHT, but if its coming from UP, it should go LEFT
             */
            i  = getIndexFromPos(curx, cury);
            it = find(case6s.begin(), case6s.end(), i);
            if (it != case6s.end())
            {
                // found, so we go down, and delete from case9s;
                stepx = -1;
                stepy = 0;
                case6s.erase(it);
            }
            else
            {
                // not found, we go up, and add to case9s;
                stepx = 1;
                stepy = 0;
                case6s.emplace_back(i);
            }
            break;
        default:
            AXLOG("this shouldn't happen.");
        }
        // little optimization
        //  if previous direction is same as current direction,
        //  then we should modify the last vec to current
        curx += stepx;
        cury += stepy;
        if (stepx == prevx && stepy == prevy)
        {
            _points.back().x = (float)(curx - rect.origin.x) / _scaleFactor;
            _points.back().y = (float)(rect.size.height - cury + rect.origin.y) / _scaleFactor;
        }
        else
        {
            _points.emplace_back(Vec2((float)(curx - rect.origin.x) / _scaleFactor, (float)(rect.size.height - cury + rect.origin.y) / _scaleFactor));
        }

        count++;
        prevx = stepx;
        prevy = stepy;

#if defined(_AX_DEBUG) && (_AX_DEBUG > 0)
        const auto totalPixel = _width * _height;
        AXASSERT(count <= totalPixel, "oh no, marching square cannot find starting position");
#endif
    } while (curx != startx || cury != starty);
    return _points;
}

float AutoPolygon::perpendicularDistance(const ax::Vec2& i, const ax::Vec2& start, const ax::Vec2& end)
{
    float res;
    float slope;
    float intercept;

    if (start.x == end.x)
    {
        res = fabsf(i.x - end.x);
    }
    else if (start.y == end.y)
    {
        res = fabsf(i.y - end.y);
    }
    else
    {
        slope     = (end.y - start.y) / (end.x - start.x);
        intercept = start.y - (slope * start.x);
        res       = fabsf(slope * i.x - i.y + intercept) / sqrtf(powf(slope, 2) + 1);
    }
    return res;
}
std::vector<ax::Vec2> AutoPolygon::rdp(const std::vector<ax::Vec2>& v, float optimization)
{
    if (v.size() < 3)
        return v;

    int index  = -1;
    float dist = 0;
    // not looping first and last point
    for (size_t i = 1, size = v.size(); i < size - 1; ++i)
    {
        float cdist = perpendicularDistance(v[i], v.front(), v.back());
        if (cdist > dist)
        {
            dist  = cdist;
            index = static_cast<int>(i);
        }
    }
    if (dist > optimization)
    {
        std::vector<Vec2>::const_iterator begin = v.begin();
        std::vector<Vec2>::const_iterator end   = v.end();
        std::vector<Vec2> l1(begin, begin + index + 1);
        std::vector<Vec2> l2(begin + index, end);

        std::vector<Vec2> r1 = rdp(l1, optimization);
        std::vector<Vec2> r2 = rdp(l2, optimization);

        r1.insert(r1.end(), r2.begin() + 1, r2.end());
        return r1;
    }
    else
    {
        std::vector<Vec2> ret;
        ret.emplace_back(v.front());
        ret.emplace_back(v.back());
        return ret;
    }
}
std::vector<Vec2> AutoPolygon::reduce(const std::vector<Vec2>& points, const Rect& rect, float epsilon)
{
    auto size = points.size();
    // if there are less than 3 points, then we have nothing
    if (size < 3)
    {
        AXLOGE("AUTOPOLYGON: cannot reduce points for {} that has less than 3 points in input, e: {}", _filename,
            epsilon);
        return std::vector<Vec2>();
    }
    // if there are less than 9 points (but more than 3), then we don't need to reduce it
    else if (size < 9)
    {
        AXLOGE("AUTOPOLYGON: cannot reduce points for {} e: {}", _filename, epsilon);
        return points;
    }
    float maxEp              = MIN(rect.size.width, rect.size.height);
    float ep                 = clampf(epsilon, 0.0, maxEp / _scaleFactor / 2);
    std::vector<Vec2> result = rdp(points, ep);

    auto last = result.back();
    if (last.y > result.front().y && last.getDistance(result.front()) < ep * 0.5f)
    {
        result.front().y = last.y;
        result.pop_back();
    }
    return result;
}

std::vector<Vec2> AutoPolygon::expand(const std::vector<Vec2>& points, const ax::Rect& rect, float epsilon)
{
    // if there are less than 3 points, then we have nothing
    if (points.size() < 3)
    {
        AXLOGE("AUTOPOLYGON: cannot expand points for {} with less than 3 points, e: {}", _filename, epsilon);
        return std::vector<Vec2>();
    }

    Clipper2Lib::Path64 result;
    Clipper2Lib::Paths64 solution;

    for (auto&& pt:points)
    {
        result.emplace_back(Clipper2Lib::Point64(pt.x * PRECISION, pt.y * PRECISION));
    }

    Clipper2Lib::ClipperOffset co;
    co.AddPath(result, Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon);
    co.Execute(epsilon * PRECISION, solution);


    // turn the result into simply polygon (AKA, fix overlap)
    // clamp into the specified rect
    Clipper2Lib::Clipper64 cl;
    Clipper2Lib::Path64 clamp;
    Clipper2Lib::Paths64 clamps;
    Clipper2Lib::PolyTree64 out;

    cl.AddSubject(solution);

    // create the clipping rect
    clamp.emplace_back(Clipper2Lib::Point64(0, 0));
    clamp.emplace_back(Clipper2Lib::Point64(int(rect.size.width / _scaleFactor * PRECISION), 0));
    clamp.emplace_back(Clipper2Lib::Point64(int(rect.size.width / _scaleFactor * PRECISION),
                                            int(rect.size.height / _scaleFactor * PRECISION)));
    clamp.emplace_back(Clipper2Lib::Point64(0, int(rect.size.height / _scaleFactor * PRECISION)));
    clamps.emplace_back(std::move(clamp));
    cl.AddClip(clamps);
    cl.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::NonZero, out);

    /*
    * Notes: the clipper2 no longer remove duplicates, so we ensure it with std::set to avoid crash some times
    *   Fix issue: https://github.com/axmolengine/axmol/issues/1075
    *   @remark: Still faster than clipper1
    */
    using Point64Ptr = const Clipper2Lib::Point64*;
    struct point64_less
    {
        bool operator()(const Point64Ptr& lhs, const Point64Ptr& rhs) const
        {
            return lhs->x < rhs->x || (lhs->x == rhs->x && lhs->y < rhs->y);
        }
    };

    std::set<Point64Ptr, point64_less> pointSets;

    std::vector<Vec2> outPoints;
    for (auto&& p2 : out)
    {
        if (!p2->IsHole())
        {
            outPoints.reserve(outPoints.size() + p2->Polygon().size());
            for (auto&& so : p2->Polygon())
            {
                if (pointSets.emplace(&so).second)
                    outPoints.emplace_back(so.x / PRECISION, so.y / PRECISION);
            }
        }
        else
        {
            AXLOG("Clipper2 detect a hole!");
        }
    }

    return outPoints;
}

TrianglesCommand::Triangles AutoPolygon::triangulate(const std::vector<Vec2>& points)
{
    // if there are less than 3 points, then we can't triangulate
    if (points.size() < 3)
    {
        AXLOGE("AUTOPOLYGON: cannot triangulate {} with less than 3 points", _filename);
        return TrianglesCommand::Triangles();
    }


    std::vector<p2t::Point> p2pointsStorage;
    p2pointsStorage.reserve(points.size());
    std::vector<p2t::Point*> p2points;
    p2points.reserve(points.size());
    for (size_t i = 0; i < points.size(); ++i)
    {
        auto& pt = points[i];
        p2points.emplace_back(&p2pointsStorage.emplace_back((double)pt.x, (double)pt.y));
    }

    p2t::CDT cdt(p2points);
    cdt.Triangulate();
    std::vector<p2t::Triangle*> tris = cdt.GetTriangles();

    axstd::pod_vector<unsigned short> indices(tris.size() * 3);
    axstd::pod_vector<V3F_C4B_T2F> verts;
    verts.reserve(indices.size() / 2);  // we won't know the size of verts until we process all of the triangles!

    unsigned short idx = 0;
    unsigned short vdx = 0;

    for (const auto& ite : tris)
    {
        for (int i = 0; i < 3; ++i)
        {
            auto p     = ite->GetPoint(i);
            auto v3    = Vec3((float)p->x, (float)p->y, 0);
            bool found = false;
            unsigned short j;
            auto length = vdx;
            for (j = 0; j < length; j++)
            {
                if (verts[j].vertices == v3)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                // if we found the same vertex, don't add to verts, but use the same vertex with indices
                indices[idx++] = j;
            }
            else
            {
                // vert does not exist yet, so we need to create a new one,
                auto c4b         = Color4B::WHITE;
                auto t2f         = Tex2F(0, 0);  // don't worry about tex coords now, we calculate that later
                verts.push_back(V3F_C4B_T2F{v3, c4b, t2f});
                indices[idx++] = vdx++;;
            }
        }
    }

    // Triangles should really use std::vector and not arrays for verts and indices.
    // Then the above memcpy would not be necessary
    TrianglesCommand::Triangles triangles = {verts.release_pointer(), indices.release_pointer(), (unsigned int)vdx,
                                             (unsigned int)idx};
    return triangles;
}

void AutoPolygon::calculateUV(const Rect& rect, V3F_C4B_T2F* verts, ssize_t count)
{
    /*
     whole texture UV coordination
     0,0                  1,0
     +---------------------+
     |                     |0.1
     |                     |0.2
     |     +--------+      |0.3
     |     |texRect |      |0.4
     |     |        |      |0.5
     |     |        |      |0.6
     |     +--------+      |0.7
     |                     |0.8
     |                     |0.9
     +---------------------+
     0,1                  1,1
     */

    AXASSERT(_width && _height, "please specify width and height for this AutoPolygon instance");
    auto texWidth  = _width;
    auto texHeight = _height;

    auto end = &verts[count];
    for (auto i = verts; i != end; ++i)
    {
        // for every point, offset with the center point
        float u        = (i->vertices.x * _scaleFactor + rect.origin.x) / texWidth;
        float v        = (rect.origin.y + rect.size.height - i->vertices.y * _scaleFactor) / texHeight;
        i->texCoords.u = u;
        i->texCoords.v = v;
    }
}

Rect AutoPolygon::getRealRect(const Rect& rect)
{
    Rect realRect = rect;
    // check rect to see if its zero
    if (realRect.equals(Rect::ZERO))
    {
        // if the instance doesn't have width and height, then the whole operation is kaput
        AXASSERT(_height && _width, "Please specify a width and height for this instance before using its functions");
        realRect = Rect(0, 0, (float)_width, (float)_height);
    }
    else
    {
        // rect is specified, so convert to real rect
        realRect = AX_RECT_POINTS_TO_PIXELS(rect);
    }
    return realRect;
}

PolygonInfo AutoPolygon::generateTriangles(const Rect& rect, float epsilon, float threshold)
{
    Rect realRect = getRealRect(rect);
    auto p        = trace(realRect, threshold);
    p             = reduce(p, realRect, epsilon);
    p             = expand(p, realRect, epsilon);
    auto tri      = triangulate(p);
    calculateUV(realRect, tri.verts, tri.vertCount);
    PolygonInfo ret;
    ret.triangles = tri;
    ret.setFilename(_filename);
    ret.setRect(realRect);
    return ret;
}

PolygonInfo AutoPolygon::generatePolygon(std::string_view filename, const Rect& rect, float epsilon, float threshold)
{
    AutoPolygon ap(filename);
    return ap.generateTriangles(rect, epsilon, threshold);
}

NS_AX_END
