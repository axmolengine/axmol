/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "Type/csmMap.hpp"
#include "Id/CubismId.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/**
 * An interface that covers the settings defined in the Model Settings File.
 *
 * @note Inherit this interface if you need to extend model settings or implement a class that uses model settings.
 */
class ICubismModelSetting
{
public:
    /**
     * Destructor
     */
    virtual ~ICubismModelSetting() {}

    /**
     * Returns the file name of MOC3 File.
     *
     * @return Name of MOC3 File
     */
    virtual const csmChar* GetModelFileName() = 0;

    /**
     * Returns the number of textures in the Model.
     *
     * @return Number of textures
     */
    virtual csmInt32 GetTextureCount() = 0;

    /**
     * Returns the name of directory that texture is placed.
     *
     * @return Name of directory
     */
    virtual const csmChar* GetTextureDirectory() = 0;

    /**
     * Returns the name of texture in the Model.
     *
     * @param index Index to the desired texture name
     *
     * @return Name of texture
     */
    virtual const csmChar* GetTextureFileName(csmInt32 index) = 0;

    /**
     * Returns the number of Hit Area settings in the Model.
     *
     * @return Number of Hit Area settings
     */
    virtual csmInt32 GetHitAreasCount() = 0;

    /**
     * Returns whether ID of Drawable that is set for the Hit Area.
     *
     * @param groupName Name of the desired Motion Group
     *
     * @return true if Motion Group information exists; otherwise false
     */
    virtual CubismIdHandle GetHitAreaId(csmInt32 index) = 0;

    /**
     * Returns the name of Hit Area.
     *
     * @param index Index to the desired Hit Area
     *
     * @return Name of Hit Area
     */
    virtual const csmChar* GetHitAreaName(csmInt32 index) = 0;

    /**
     * Returns the name of Physics Settings File in the Model.
     *
     * @return Name of Physics Settings File
     */
    virtual const csmChar* GetPhysicsFileName() = 0;

    /**
     * Returns the name of Pose Settings File in the Model.
     *
     * @return Name of Pose Settings File
     */
    virtual const csmChar* GetPoseFileName() = 0;

    /**
     * Returns the name of Display Settings File in the Model.
     *
     * @return Name of Display Settings File
     */
    virtual const csmChar* GetDisplayInfoFileName() = 0;

    /**
     * Returns the number of expressions in the Model.
     *
     * @return Number of expressions
     */
    virtual csmInt32 GetExpressionCount() = 0;

    /**
     * Returns the name of expression.
     *
     * @param index Index to the desired expression
     *
     * @return Name of expression
     */
    virtual const csmChar* GetExpressionName(csmInt32 index) = 0;

    /**
     * Returns the name of Expression Settings File from the expression.
     *
     * @param index Index to the desired expression
     *
     * @return Name of Expression Settings File
     */
    virtual const csmChar* GetExpressionFileName(csmInt32 index) = 0;

    /**
     * Returns the number of Motion Groups in the Model.
     *
     * @return Number of Motion Groups
     */
    virtual csmInt32 GetMotionGroupCount() = 0;

    /**
     * Returns the name of Motion Group.
     *
     * @param index Index to the desired Motion Group
     *
     * @return Name of Motion Group
     */
    virtual const csmChar* GetMotionGroupName(csmInt32 index) = 0;

    /**
     * Returns the number of Motions in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     *
     * @return Number of Motions
     */
    virtual csmInt32 GetMotionCount(const csmChar* groupName) = 0;

    /**
     * Returns the file name of Motion File in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired motion
     *
     * @return Name of Motion File
     */
    virtual const csmChar* GetMotionFileName(const csmChar* groupName, csmInt32 index) = 0;

    /**
     * Returns the name of Audio File attached to the Motion in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return Name of Audio File
     */
    virtual const csmChar* GetMotionSoundFileName(const csmChar* groupName, csmInt32 index) = 0;

    /**
     * Returns the Fade-in time at the start of the Motion in the Motion Group
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return Fade-in time [sec]
     */
    virtual csmFloat32 GetMotionFadeInTimeValue(const csmChar* groupName, csmInt32 index) = 0;

    /**
     * Returns the Fade-out time at the end of the Motion in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return Fade-out time [sec]
     */
    virtual csmFloat32 GetMotionFadeOutTimeValue(const csmChar* groupName, csmInt32 index) = 0;

    /**
     * Returns the name of User Data File in the Model.
     *
     * @return Name of User Data File
     */
    virtual const csmChar* GetUserDataFile() = 0;

    /**
     * Returns the Layout Information set in the model.
     *
     * @param outLayoutMap Instance of a collection to store
     *
     * @return true if Layout Information exists; otherwise false
     */
    virtual csmBool GetLayoutMap(csmMap<csmString, csmFloat32>& outLayoutMap) = 0;

    /**
     * Returns the number of parameters attached to the Eye Blinking Settings.
     *
     * @return Number of parameters
     */
    virtual csmInt32 GetEyeBlinkParameterCount() = 0;

    /**
     * Returns the ID of parameter attached to the Eye Blinking Settings.
     *
     * @param index Index to the desired parameter
     *
     * @return ID of parameter
     */
    virtual CubismIdHandle GetEyeBlinkParameterId(csmInt32 index) = 0;

    /**
     * Returns the number of parameters attached to the Lip-sync Settings.
     *
     * @return Number of parameters
     */
    virtual csmInt32 GetLipSyncParameterCount() = 0;

    /**
     * Returns the ID of parameter attached to the Lip-sync Settings.
     *
     * @param index Index to the desired parameter
     *
     * @return ID of parameter
     */
    virtual CubismIdHandle GetLipSyncParameterId(csmInt32 index) = 0;
};
}}}
