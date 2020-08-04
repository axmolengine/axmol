#ifndef __GPATH_H__
#define __GPATH_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

struct GPathPoint
{
    enum class CurveType
    {
        CRSpline,
        Bezier,
        CubicBezier,
        Straight
    };

    cocos2d::Vec3 pos;
    cocos2d::Vec3 control1;
    cocos2d::Vec3 control2;
    CurveType curveType;

    GPathPoint(const cocos2d::Vec3& pos);
    GPathPoint(const cocos2d::Vec3& pos, const cocos2d::Vec3& control);
    GPathPoint(const cocos2d::Vec3& pos, const cocos2d::Vec3& control1, const cocos2d::Vec3& control2);
    GPathPoint(const cocos2d::Vec3& pos, CurveType curveType);
};

class GPath
{
public:
    GPath();
    void create(GPathPoint* points, int count);
    void clear();
    cocos2d::Vec3 getPointAt(float t);

    float getLength() { return _fullLength; }
    int getSegmentCount() { return (int)_segments.size(); }
    float getSegmentLength(int segmentIndex);
    void getPointsInSegment(int segmentIndex, float t0, float t1,
                            std::vector<cocos2d::Vec3>& points, std::vector<float>* ts = nullptr, float pointDensity = 0.1f);
    void getAllPoints(std::vector<cocos2d::Vec3>& points, float pointDensity = 0.1f);

    struct Segment
    {
        GPathPoint::CurveType type;
        float length;
        int ptStart;
        int ptCount;
    };

private:
    void createSplineSegment();
    cocos2d::Vec3 onCRSplineCurve(int ptStart, int ptCount, float t);
    cocos2d::Vec3 onBezierCurve(int ptStart, int ptCount, float t);

    std::vector<Segment> _segments;
    std::vector<cocos2d::Vec3> _points;
    float _fullLength;
};

NS_FGUI_END

#endif
