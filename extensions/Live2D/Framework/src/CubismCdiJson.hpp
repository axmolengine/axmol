/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D {  namespace Cubism {  namespace Framework {

/**
 * Handles the Display Information File.
 */
class CubismCdiJson : public CubismJsonHolder
{
public:
    /**
     * Constructor<br>
     * Loads the Display Information File.
     *
     * @param buffer Buffer that Display Settings File is loaded from.
     * @param size Number of bytes in buffer
     */
    CubismCdiJson(const csmByte* buffer, csmSizeInt size);

    /**
    　* Destructor
    　*/
    virtual ~CubismCdiJson();

    /**
     * Returns the number of parameters.
     *
     * @return Number of parameters
     */
    csmInt32 GetParametersCount();

    /**
     * Returns the ID of the parameter.
     *
     * @param index Index to the desired parameter
     *
     * @return ID of the parameter
     */
    const csmChar* GetParametersId(csmInt32 index);

    /**
     * Returns the ID of the parameter group that the parameter belongs to.
     *
     * @param index Index to the desired parameter
     *
     * @return ID of the parameter group
     */
    const csmChar* GetParametersGroupId(csmInt32 index);

    /**
     * Returns the name of the parameter.
     *
     * @param index Index to the desired parameter
     *
     * @return name of parameter
     */
    const csmChar* GetParametersName(csmInt32 index);

    /**
     * Returns the number of parameter groups.
     *
     * @return Number of parameter groups
     */
    csmInt32 GetParameterGroupsCount();

    /**
     * Returns the ID of the parameter group.
     *
     * @param index Index to the desired parameter group
     *
     * @return ID of parameter group
     */
    const csmChar* GetParameterGroupsId(csmInt32 index);

    /**
     * Returns the ID of the parameter group that the parameter group belongs to.
     *
     * @param index Index to the desired parameter group
     *
     * @return ID of the parameter group
     */
    const csmChar* GetParameterGroupsGroupId(csmInt32 index);

    /**
     * Returns the name of the parameter group.
     *
     * @param index Index to the desired parameter group
     *
     * @return Name of the parameter group
     */
    const csmChar* GetParameterGroupsName(csmInt32 index);

    /**
     * Returns the number of parts.
     *
     * @return Number of parts
     */
    csmInt32 GetPartsCount();

    /**
     * Returns the ID of the part.
     *
     * @param index Index to the desired part
     *
     * @return ID of the part
     */
    const csmChar* GetPartsId(csmInt32 index);

    /**
     * Returns the name of the part.
     *
     * @param index Index to the desired part
     *
     * @return name of the part
     */
    const csmChar* GetPartsName(csmInt32 index);

    /**
     * Returns the number of combined parameters.
     *
     * @return Number of combined parameters
     */
    csmInt32 GetCombinedParametersCount();

    /**
     * Returns the pair list of the combined parameters.
     *
     * @param index Index to the desired combined parameter
     *
     * @return Pair list of the combined parameter
     */
    const csmVector<Utils::Value*>* GetCombinedParameters(csmInt32 index);

private:
    /**
     * Returns whether the parameters exist in the Display Information File.
     *
     * @return true if parameters exists; otherwise false.
     */
    csmBool IsExistParameters() const;

    /**
     * Returns whether parameter groups exist in the Display Information File.
     *
     * @return true if parameters on parameter groups exist; otherwise false.
     */
    csmBool IsExistParameterGroups() const;

    /**
     * Returns whether the parts exist in the Display Information File.
     *
     * @return true if parts exist; otherwise false.
     */
    csmBool IsExistParts() const;

    /**
     * Returns whether the combined parameters exist in the Display Information File.
     *
     * @return true if combined parameters exist; otherwise false.
     */
    csmBool IsExistCombinedParameters() const;
};

}}}
