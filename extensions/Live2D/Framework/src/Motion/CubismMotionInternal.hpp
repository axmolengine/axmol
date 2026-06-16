/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Types of motion curve application targets
 */
enum CubismMotionCurveTarget
{
    CubismMotionCurveTarget_Model,          ///< Applied to model
    CubismMotionCurveTarget_Parameter,      ///< Applied to parameter
    CubismMotionCurveTarget_PartOpacity     ///< Applied to part opacity
};

/**
 * Types of motion curve segments
 */
enum CubismMotionSegmentType
{
    CubismMotionSegmentType_Linear = 0,             ///< Linear
    CubismMotionSegmentType_Bezier = 1,             ///< Bezier curve
    CubismMotionSegmentType_Stepped = 2,            ///< Step
    CubismMotionSegmentType_InverseStepped = 3      ///< Inverse step
};

/**
 * Data for control points of motion curve
 */
struct CubismMotionPoint
{
    /**
     * Constructor
     */
    CubismMotionPoint()
        : Time(0.0f)
        , Value(0.0f)
    { }

    csmFloat32 Time;        ///< Time [seconds]
    csmFloat32 Value;       ///< Value
};

/**
 * Function declaration for evaluating motion curve segments
 *
 * @param points Collection of control points for the motion curve
 * @param time Time to evaluate [seconds]
 */
typedef csmFloat32 (*csmMotionSegmentEvaluationFunction)(const CubismMotionPoint* points, const csmFloat32 time);

/**
 * Data for motion curve segments
 */
struct CubismMotionSegment
{
    /**
     * Constructor
     */
    CubismMotionSegment()
        : Evaluate(NULL)
        , BasePointIndex(0)
        , SegmentType(0)
    { }

    csmMotionSegmentEvaluationFunction Evaluate;        ///< Function to evaluate segment
    csmInt32 BasePointIndex;                            ///< Index of the first control point
    csmInt32 SegmentType;                               ///< Segment type
};

/**
 * Data for motion curve
 */
struct CubismMotionCurve
{
    /**
     * Constructor
     */
    CubismMotionCurve()
        : Type(CubismMotionCurveTarget_Model)
        , SegmentCount(0)
        , BaseSegmentIndex(0)
        , FadeInTime(0.0f)
        , FadeOutTime(0.0f)
    { }

    CubismMotionCurveTarget Type;       ///< Curve type
    CubismIdHandle Id;                  ///< ID of the parameter attached to the curve
    csmInt32 SegmentCount;              ///< Number of segments
    csmInt32 BaseSegmentIndex;          ///< Index of the first segment
    csmFloat32 FadeInTime;              ///< Seconds to complete fade-in from start to finish [seconds]
    csmFloat32 FadeOutTime;             ///< Seconds to complete fade-out from start to finish [seconds]
};

/**
 * Data for user data events
 */
struct CubismMotionEvent
{
    /**
     * Constructor
     */
    CubismMotionEvent()
        : FireTime(0.0f)
    { }

    csmFloat32  FireTime;       ///< Seconds in motion when the event fires [seconds]
    csmString   Value;          ///< Value
};

/**
 * Data for motion
 */
struct CubismMotionData
{
    /**
     * Constructor
     */
    CubismMotionData()
        : Duration(0.0f)
        , Loop(0)
        , CurveCount(0)
        , EventCount(0)
        , Fps(0.0f)
    { }

    csmFloat32 Duration;                            ///< Motion length [seconds]
    csmInt16 Loop;                                  ///< Whether to loop
    csmInt16 CurveCount;                            ///< Number of curves
    csmInt32 EventCount;                            ///< Number of user data events
    csmFloat32 Fps;                                 ///< Motion frame rate
    csmVector<CubismMotionCurve> Curves;            ///< Curve collection
    csmVector<CubismMotionSegment> Segments;        ///< Segment collection
    csmVector<CubismMotionPoint> Points;            ///< Control point collection
    csmVector<CubismMotionEvent> Events;            ///< User data event collection
};

}}}
