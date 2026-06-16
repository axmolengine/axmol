/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

enum EvaluationOptionFlag
{
    EvaluationOptionFlag_AreBeziersRestricted = 0,
};

/**
 * Handles motion files.
 */
class CubismMotionJson : public CubismJsonHolder
{
public:
    /**
     * Constructor<br>
     * Loads the motion file.
     *
     * @param buffer buffer containing the loaded motion file
     * @param size size of the buffer in bytes
     */
    CubismMotionJson(const csmByte* buffer, csmSizeInt size);

    /**
     * Destructor
     */
    virtual ~CubismMotionJson();

    /**
     * Returns the length of the motion.
     *
     * @return length of the motion in seconds
     */
    csmFloat32 GetMotionDuration() const;

    /**
     * Checks whether the motion is set to loop.
     *
     * @return true if the motion is set to loop; otherwise false.
     */
    csmBool IsMotionLoop() const;

    /**
     *  Returns the consistency of the motion3.json file.
     *
     * @return true if the file is consistent; otherwise returns false.
     */
    csmBool HasConsistency() const;

     /**
     * Returns the option settings used during the motion curve evaluation.
     *
     * @param Type option to retrieve
     *
     * @return option setting; true if enabled
     *
     * @note Use EvaluationOptionFlag to specify the option to retrieve.
     */
    csmBool GetEvaluationOptionFlag(csmInt32 flagType) const;

    /**
     * Returns the number of curves in the motion.
     *
     * @return number of curves in the motion
     */
    csmInt32 GetMotionCurveCount() const;

    /**
     * Returns the frame rate of the motion.
     *
     * @return frame rate of the motion in FPS
     */
    csmFloat32 GetMotionFps() const;

    /**
     * Returns the total number of segments in the motion.
     *
     * @return total number of segments in the motion
     */
    csmInt32 GetMotionTotalSegmentCount() const;

    /**
     * Returns the total number of control points in the motion curves.
     *
     * @return total number of control points in the motion curves
     */
    csmInt32 GetMotionTotalPointCount() const;

    /**
     * Checks whether the setting for the number of seconds to complete fading in exists.
     *
     * @return true if the setting exists; otherwise false.
     */
    csmBool IsExistMotionFadeInTime() const;

    /**
     * Checks whether the setting for the number of seconds to complete fading out exists.
     *
     * @return true if the setting exists; otherwise false.
     */
    csmBool IsExistMotionFadeOutTime() const;

    /**
     * Returns the number of seconds for the motion to complete fading in.
     *
     * @return number of seconds for the fade-in to complete
     */
    csmFloat32 GetMotionFadeInTime() const;

    /**
     * Returns the number of seconds for the motion to complete fading out.
     *
     * @return number of seconds for the fade-out to complete
     */
    csmFloat32 GetMotionFadeOutTime() const;

    /**
     * Returns the destination type of the motion curve.
     *
     * @param curveIndex index of the curve to retrieve
     *
     * @return destination type of the curve
     */
    const csmChar* GetMotionCurveTarget(csmInt32 curveIndex) const;

    /**
     * Returns the ID of the parameter attached to the motion curve.
     *
     * @param curveIndex index of the curve to retrieve
     *
     * @return ID of the attached parameter
     */
    CubismIdHandle GetMotionCurveId(csmInt32 curveIndex) const;

    /**
     * Checks whether the setting for the number of seconds to complete fading in for the motion curve exists.
     *
     * @param curveIndex index of the curve to check
     *
     * @return true if the setting exists; otherwise false.
     */
    csmBool IsExistMotionCurveFadeInTime(csmInt32 curveIndex) const;

    /**
     * Checks whether the setting for the number of seconds to complete fading out for the motion curve exists.
     *
     * @param curveIndex index of the curve to check
     *
     * @return true if the setting exists; otherwise false.
     */
    csmBool IsExistMotionCurveFadeOutTime(csmInt32 curveIndex) const;

    /**
     * Returns the number of seconds for the motion curve to complete fading in.
     *
     * @return number of seconds for the fade-in to complete
     */
    csmFloat32 GetMotionCurveFadeInTime(csmInt32 curveIndex) const;

    /**
     * Returns the number of seconds for the motion to complete fading out.
     *
     * @return number of seconds for the fade-out to complete
     */
    csmFloat32 GetMotionCurveFadeOutTime(csmInt32 curveIndex) const;

    /**
     * Returns the number of segments in the motion curve.
     *
     * @param curveIndex index of the curve to retrieve
     *
     * @return number of segments in the curve
     */
    csmInt32 GetMotionCurveSegmentCount(csmInt32 curveIndex) const;


    /**
     * Returns the value of the segment in the motion curve.
     *
     * @param curveIndex index of the curve to retrieve
     * @param segmentIndex index of the segment to retrieve
     *
     * @return value of the segment in the curve
     */
    csmFloat32 GetMotionCurveSegment(csmInt32 curveIndex, csmInt32 segmentIndex) const;

    /**
     * Returns the number of user data events in the motion.
     *
     * @return number of user data events
     */
    csmInt32 GetEventCount() const;

    /**
     * Returns the number of user data events in the motion.
     *
     * @return number of user data events
     */
    csmInt32 GetTotalEventValueSize() const;

    /**
     * Returns the motion time in seconds when the user data event fires.
     *
     * @param userDataIndex index of the user data event to retrieve
     *
     * @return motion time in seconds when the user data event fires
     */
    csmFloat32 GetEventTime(csmInt32 userDataIndex) const;

    /**
     * Returns the value of the user data event in the motion.
     *
     * @param userDataIndex index of the user data event to retrieve
     *
     * @return value of the user data event
     */
    const csmChar* GetEventValue(csmInt32 userDataIndex) const;
};

}}}
