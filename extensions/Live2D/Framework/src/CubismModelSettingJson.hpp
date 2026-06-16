/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismModelSetting.hpp"
#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Handles the Model Settings File
 */
class CubismModelSettingJson : public ICubismModelSetting, public CubismJsonHolder
{
public:

    /**
     * Constructor<br>
     * Loads the Model Settings File.
     *
     * @param buffer Buffer into which the Model Settings File is loaded
     * @param size Number of bytes in buffer
     */
    CubismModelSettingJson(const csmByte* buffer, csmSizeInt size);

    /**
     * Destructor
     */
    virtual ~CubismModelSettingJson();

    /**
     * Returns the instance of CubismJson that handles the JSON document.
     *
     * @return Instance of CubismJson
     */
    Utils::CubismJson* GetJsonPointer() const;

    /**
     * Returns the file name of MOC3 File.
     *
     * @return Name of MOC3 File
     */
    const csmChar* GetModelFileName();

    /**
     * Returns the number of textures in the Model.
     *
     * @return Number of textures
     */
    csmInt32 GetTextureCount();

    /**
     * Returns the name of directory that texture is placed.
     *
     * @return Name of directory
     */
    const csmChar* GetTextureDirectory();

    /**
     * Returns the name of texture in the Model.
     *
     * @param index Index to the desired texture name
     *
     * @return Name of texture
     */
    const csmChar* GetTextureFileName(csmInt32 index);

    /**
     * Returns the number of Hit Area settings in the Model.
     *
     * @return Number of Hit Area settings
     */
    csmInt32 GetHitAreasCount();

    /**
     * Returns whether ID of Drawable that is set for the Hit Area.
     *
     * @param groupName Name of the desired Motion Group
     *
     * @return true if Motion Group information exists; otherwise false
     */
    CubismIdHandle GetHitAreaId(csmInt32 index);

    /**
     * Returns the name of Hit Area.
     *
     * @param index Index to the desired Hit Area
     *
     * @return Name of Hit Area
     */
    const csmChar* GetHitAreaName(csmInt32 index);

    /**
     * Returns the name of Physics Settings File in the Model.
     *
     * @return Name of Physics Settings File
     */
    const csmChar* GetPhysicsFileName();

    /**
     * Returns the name of Pose Settings File in the Model.
     *
     * @return Name of Pose Settings File
     */
    const csmChar* GetPoseFileName();

    /**
     * Returns the name of Display Settings File in the Model.
     *
     * @return Name of Display Settings File
     */
    const csmChar* GetDisplayInfoFileName();

    /**
     * Returns the number of expressions in the Model.
     *
     * @return Number of expressions
     */
    csmInt32 GetExpressionCount();

    /**
     * Returns the name of expression.
     *
     * @param index Index to the desired expression
     *
     * @return Name of expression
     */
    const csmChar* GetExpressionName(csmInt32 index);

    /**
     * Returns the name of Expression Settings File from the expression.
     *
     * @param index Index to the desired expression
     *
     * @return Name of Expression Settings File
     */
    const csmChar* GetExpressionFileName(csmInt32 index);

    /**
     * Returns the number of Motion Groups in the Model.
     *
     * @return Number of Motion Groups
     */
    csmInt32 GetMotionGroupCount();

    /**
     * Returns the name of Motion Group.
     *
     * @param index Index to the desired Motion Group
     *
     * @return Name of Motion Group
     */
    const csmChar* GetMotionGroupName(csmInt32 index);

    /**
     * Returns the number of Motions in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     *
     * @return Number of Motions
     */
    csmInt32 GetMotionCount(const csmChar* groupName);

    /**
     * Returns the file name of Motion File in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired motion
     *
     * @return Name of Motion File
     */
    const csmChar* GetMotionFileName(const csmChar* groupName, csmInt32 index);

    /**
     * Returns the name of Audio File attached to the Motion in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return Name of Audio File
     */
    const csmChar* GetMotionSoundFileName(const csmChar* groupName, csmInt32 index);

    /**
     * Returns the Fade-in time at the start of the Motion in the Motion Group
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return Fade-in time [sec]
     */
    csmFloat32 GetMotionFadeInTimeValue(const csmChar* groupName, csmInt32 index);

    /**
     * Returns the Fade-out time at the end of the Motion in the Motion Group.
     *
     * @param groupName Name to the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return Fade-out time [sec]
     */
    csmFloat32 GetMotionFadeOutTimeValue(const csmChar* groupName, csmInt32 index);

    /**
     * Returns the name of User Data File in the Model.
     *
     * @return Name of User Data File
     */
    const csmChar* GetUserDataFile();

    /**
     * Returns the Layout Information set in the model.
     *
     * @param outLayoutMap Instance of a collection to store
     *
     * @return true if Layout Information exists; otherwise false
     */
    csmBool GetLayoutMap(csmMap<csmString, csmFloat32>& outLayoutMap);

    /**
     * Returns the number of parameters attached to the Eye Blinking Settings.
     *
     * @return Number of parameters
     */
    csmInt32 GetEyeBlinkParameterCount();

    /**
     * Returns the ID of parameter attached to the Eye Blinking Settings.
     *
     * @param index Index to the desired parameter
     *
     * @return ID of parameter
     */
    CubismIdHandle GetEyeBlinkParameterId(csmInt32 index);

    /**
     * Returns the number of parameters attached to the Lip-sync Settings.
     *
     * @return Number of parameters
     */
    csmInt32 GetLipSyncParameterCount();

    /**
     * Returns the ID of parameter attached to the Lip-sync Settings.
     *
     * @param index Index to the desired parameter
     *
     * @return ID of parameter
     */
    CubismIdHandle GetLipSyncParameterId(csmInt32 index);

protected:

    enum FrequentNode
    {
        FrequentNode_Groups,
        FrequentNode_Moc,
        FrequentNode_Motions,
        FrequentNode_DisplayInfo,
        FrequentNode_Expressions,
        FrequentNode_Textures,
        FrequentNode_Physics,
        FrequentNode_Pose,
        FrequentNode_HitAreas,
    };

    /**
     * Returns whether the MOC3 File information exists in the Model Settings File.
     *
     * @return true if MOC3 File information exists; otherwise false
     */
    csmBool IsExistModelFile() const;

    /**
     * Returns whether Texture File information exists in the Model Settings File.
     *
     * @return true if Texture File information exists; otherwise false
     */
    csmBool IsExistTextureFiles() const;

    /**
     * Returns whether Hit Area information exists in the Model Settings File.
     *
     * @return true if Hit Area information exists; otherwise false
     */
    csmBool IsExistHitAreas() const;

    /**
     * Returns whether Physics Settings File information exists in the Model Settings File.
     *
     * @return true if Physics Settings File information exists; otherwise false
     */
    csmBool IsExistPhysicsFile() const;

    /**
     * Returns whether Pose Settings File information exists in the Model Settings File.
     *
     * @return true if Pose Settings File information exists; otherwise false
     */
    csmBool IsExistPoseFile() const;

    /**
     * Returns whether Display Settings File information exists in the Model Settings File.
     *
     * @return true if Display Settings File information exists; otherwise false
     */
    csmBool IsExistDisplayInfoFile() const;

    /**
     * Returns whether Expression File information exists in the Model Settings File.
     *
     * @return true if Expression File information exists; otherwise false
     */
    csmBool IsExistExpressionFile() const;

    /**
     * Returns whether Motion Group information exists in the Model Settings File.
     *
     * @return true if Motion Group information exists; otherwise false
     */
    csmBool IsExistMotionGroups() const;

    /**
     * Returns the Motion Group information with the specified name in the Model Settings File.
     *
     * @param groupName Name of the desired Motion Group
     *
     * @return true if Motion Group information exists; otherwise false
     */
    csmBool IsExistMotionGroupName(const csmChar* groupName) const;

    /**
     * Returns whether the Audio File information in the Motion Group with the specified name in the Model Settings File.
     *
     * @param groupName Name of the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return true if Audio File information exists; otherwise false
     */
    csmBool IsExistMotionSoundFile(const csmChar* groupName, csmInt32 index) const;

    /**
     * Returns whether the Fade-in time at the start of Motion in Motion Group with the specified name exists in Model Setting File.
     *
     * @param groupName Name of the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return true if Fade-in time exists; otherwise false
     */
    csmBool IsExistMotionFadeIn(const csmChar* groupName, csmInt32 index) const;

    /**
     * Returns whether the Fade-out time at the end of the Motion in the Motion Group with the specified name in the Model Settings File.
     *
     * @param groupName Name of the desired Motion Group
     * @param index Index to the desired Motion
     *
     * @return true if Fade-out time exists; otherwise false
     */
    csmBool IsExistMotionFadeOut(const csmChar* groupName, csmInt32 index) const;

    /**
     * Returns whether the User Data File information exists in the Model Settings File.
     *
     * @return true if User Data File information exists; otherwise false
     */
    csmBool IsExistUserDataFile() const;

    /**
     * Returns whether the parameter information attached to the Eye Blinking Settings exists in the Model Settings File.
     *
     * @return true if parameter information exists; otherwise false
     */
    csmBool IsExistEyeBlinkParameters() const;

    /**
     * Returns whether the parameter information attached to the Lip-sync Settings exists in the Model Settings File.
     *
     * @return true if parameter information exists; otherwise false
     */
    csmBool IsExistLipSyncParameters() const;

    /** Cache of JSON nodes */
    csmVector<Utils::Value*>    _jsonValue;
};
}}}
