#include "GPath.h"

NS_FGUI_BEGIN
using namespace ax;

static std::vector<ax::Vec3> splinePoints;

GPathPoint::GPathPoint(const Vec3& pos)
{
    this->pos = pos;
    this->control1 = Vec3::ZERO;
    this->control2 = Vec3::ZERO;
    this->curveType = CurveType::CRSpline;
}

GPathPoint::GPathPoint(const Vec3& pos, const Vec3& control)
{
    this->pos = pos;
    this->control1 = control;
    this->control2 = Vec3::ZERO;
    this->curveType = CurveType::Bezier;
}

GPathPoint::GPathPoint(const Vec3& pos, const Vec3& control1, const Vec3& control2)
{
    this->pos = pos;
    this->control1 = control1;
    this->control2 = control2;
    this->curveType = CurveType::CubicBezier;
}

GPathPoint::GPathPoint(const Vec3& pos, CurveType curveType)
{
    this->pos = pos;
    this->control1 = Vec3::ZERO;
    this->control2 = Vec3::ZERO;
    this->curveType = curveType;
}

GPath::GPath()
    : _fullLength(0)
{
}

void GPath::create(GPathPoint* points, int count)
{
    _segments.clear();
    _points.clear();
    splinePoints.clear();
    _fullLength = 0;

    if (count == 0)
        return;

    const GPathPoint* prev = points;
    if (prev->curveType == GPathPoint::CurveType::CRSpline)
        splinePoints.push_back(prev->pos);

    for (int i = 1; i < count; i++)
    {
        const GPathPoint* current = points + i;

        if (prev->curveType != GPathPoint::CurveType::CRSpline)
        {
            Segment seg;
            seg.type = prev->curveType;
            seg.ptStart = (int)_points.size();
            if (prev->curveType == GPathPoint::CurveType::Straight)
            {
                seg.ptCount = 2;
                _points.push_back(prev->pos);
                _points.push_back(current->pos);
            }
            else if (prev->curveType == GPathPoint::CurveType::Bezier)
            {
                seg.ptCount = 3;
                _points.push_back(prev->pos);
                _points.push_back(current->pos);
                _points.push_back(prev->control1);
            }
            else if (prev->curveType == GPathPoint::CurveType::CubicBezier)
            {
                seg.ptCount = 4;
                _points.push_back(prev->pos);
                _points.push_back(current->pos);
                _points.push_back(prev->control1);
                _points.push_back(prev->control2);
            }
            seg.length = prev->pos.distance(current->pos);
            _fullLength += seg.length;
            _segments.push_back(seg);
        }

        if (current->curveType != GPathPoint::CurveType::CRSpline)
        {
            if (splinePoints.size() > 0)
            {
                splinePoints.push_back(current->pos);
                createSplineSegment();
            }
        }
        else
            splinePoints.push_back(current->pos);

        prev = current;
    }

    if (splinePoints.size() > 1)
        createSplineSegment();
}

void GPath::createSplineSegment()
{
    int cnt = (int)splinePoints.size();
    splinePoints.insert(splinePoints.begin(), splinePoints[0]);
    splinePoints.push_back(splinePoints[cnt]);
    splinePoints.push_back(splinePoints[cnt]);
    cnt += 3;

    Segment seg;
    seg.type = GPathPoint::CurveType::CRSpline;
    seg.ptStart = (int)_points.size();
    seg.ptCount = cnt;
    for (auto& it : splinePoints)
        _points.push_back(it);

    seg.length = 0;
    for (int i = 1; i < cnt; i++)
        seg.length += splinePoints[i - 1].distance(splinePoints[i]);
    _fullLength += seg.length;
    _segments.push_back(seg);
    splinePoints.clear();
}

void GPath::clear()
{
    _segments.clear();
    _points.clear();
}

Vec3 GPath::getPointAt(float t)
{
    t = clampf(t, 0, 1);
    int cnt = (int)_segments.size();
    if (cnt == 0)
        return Vec3::ZERO;

    Segment seg;
    if (t == 1)
    {
        seg = _segments[cnt - 1];

        if (seg.type == GPathPoint::CurveType::Straight)
            return _points[seg.ptStart].lerp(_points[seg.ptStart + 1], t);
        else if (seg.type == GPathPoint::CurveType::Bezier || seg.type == GPathPoint::CurveType::CubicBezier)
            return onBezierCurve(seg.ptStart, seg.ptCount, t);
        else
            return onCRSplineCurve(seg.ptStart, seg.ptCount, t);
    }

    float len = t * _fullLength;
    Vec3 pt;
    for (int i = 0; i < cnt; i++)
    {
        seg = _segments[i];

        len -= seg.length;
        if (len < 0)
        {
            t = 1 + len / seg.length;

            if (seg.type == GPathPoint::CurveType::Straight)
                pt = _points[seg.ptStart].lerp(_points[seg.ptStart + 1], t);
            else if (seg.type == GPathPoint::CurveType::Bezier || seg.type == GPathPoint::CurveType::CubicBezier)
                pt = onBezierCurve(seg.ptStart, seg.ptCount, t);
            else
                pt = onCRSplineCurve(seg.ptStart, seg.ptCount, t);

            break;
        }
    }

    return pt;
}

float GPath::getSegmentLength(int segmentIndex)
{
    return _segments[segmentIndex].length;
}

void GPath::getPointsInSegment(int segmentIndex, float t0, float t1,
                               std::vector<ax::Vec3>& points, std::vector<float>* ts, float pointDensity)
{
    if (ts != nullptr)
        ts->push_back(t0);
    Segment seg = _segments[segmentIndex];
    if (seg.type == GPathPoint::CurveType::Straight)
    {
        points.push_back(_points[seg.ptStart].lerp(_points[seg.ptStart + 1], t0));
        points.push_back(_points[seg.ptStart].lerp(_points[seg.ptStart + 1], t1));
    }
    else if (seg.type == GPathPoint::CurveType::Bezier || seg.type == GPathPoint::CurveType::CubicBezier)
    {
        points.push_back(onBezierCurve(seg.ptStart, seg.ptCount, t0));
        int SmoothAmount = (int)MIN(seg.length * pointDensity, 50);
        for (int j = 0; j <= SmoothAmount; j++)
        {
            float t = (float)j / SmoothAmount;
            if (t > t0 && t < t1)
            {
                points.push_back(onBezierCurve(seg.ptStart, seg.ptCount, t));
                if (ts != nullptr)
                    ts->push_back(t);
            }
        }
        points.push_back(onBezierCurve(seg.ptStart, seg.ptCount, t1));
    }
    else
    {
        points.push_back(onCRSplineCurve(seg.ptStart, seg.ptCount, t0));
        int SmoothAmount = (int)MIN(seg.length * pointDensity, 50);
        for (int j = 0; j <= SmoothAmount; j++)
        {
            float t = (float)j / SmoothAmount;
            if (t > t0 && t < t1)
            {
                points.push_back(onCRSplineCurve(seg.ptStart, seg.ptCount, t));
                if (ts != nullptr)
                    ts->push_back(t);
            }
        }
        points.push_back(onCRSplineCurve(seg.ptStart, seg.ptCount, t1));
    }

    if (ts != nullptr)
        ts->push_back(t1);
}

void GPath::getAllPoints(std::vector<ax::Vec3>& points, float pointDensity)
{
    int cnt = (int)_segments.size();
    for (int i = 0; i < cnt; i++)
        getPointsInSegment(i, 0, 1, points, nullptr, pointDensity);
}

static float repeat(float t, float length)
{
    return t - floor(t / length) * length;
}

Vec3 GPath::onCRSplineCurve(int ptStart, int ptCount, float t)
{
    int adjustedIndex = floor(t * (ptCount - 4)) + ptStart; //Since the equation works with 4 points, we adjust the starting point depending on t to return a point on the specific segment

    Vec3 result;

    Vec3 p0 = _points[adjustedIndex];
    Vec3 p1 = _points[adjustedIndex + 1];
    Vec3 p2 = _points[adjustedIndex + 2];
    Vec3 p3 = _points[adjustedIndex + 3];

    float adjustedT = (t == 1.f) ? 1.f : repeat(t * (ptCount - 4), 1.f); // Then we adjust t to be that value on that new piece of segment... for t == 1f don't use repeat (that would return 0f);

    float t0 = ((-adjustedT + 2.f) * adjustedT - 1.f) * adjustedT * 0.5f;
    float t1 = (((3.f * adjustedT - 5.f) * adjustedT) * adjustedT + 2.f) * 0.5f;
    float t2 = ((-3.f * adjustedT + 4.f) * adjustedT + 1.f) * adjustedT * 0.5f;
    float t3 = ((adjustedT - 1.f) * adjustedT * adjustedT) * 0.5f;

    result.x = p0.x * t0 + p1.x * t1 + p2.x * t2 + p3.x * t3;
    result.y = p0.y * t0 + p1.y * t1 + p2.y * t2 + p3.y * t3;
    result.z = p0.z * t0 + p1.z * t1 + p2.z * t2 + p3.z * t3;

    return result;
}

Vec3 GPath::onBezierCurve(int ptStart, int ptCount, float t)
{
    float t2 = 1.0f - t;
    Vec3 p0 = _points[ptStart];
    Vec3 p1 = _points[ptStart + 1];
    Vec3 cp0 = _points[ptStart + 2];

    if (ptCount == 4)
    {
        Vec3 cp1 = _points[ptStart + 3];
        return t2 * t2 * t2 * p0 + 3.f * t2 * t2 * t * cp0 + 3.f * t2 * t * t * cp1 + t * t * t * p1;
    }
    else
        return t2 * t2 * p0 + 2.f * t2 * t * cp0 + t * t * p1;
}

NS_FGUI_END