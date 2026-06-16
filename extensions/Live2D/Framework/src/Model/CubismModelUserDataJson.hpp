/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Model/CubismModel.hpp"
#include "Id/CubismIdManager.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D {  namespace Cubism {  namespace Framework {

/**
 * Handles user data.
 */
class CubismModelUserDataJson : public CubismJsonHolder
{
public:
    /**
     * Constructor
     *
     * @param buffer Buffer where the user data file is loaded
     * @param size Number of bytes in the buffer
     */
    CubismModelUserDataJson(const csmByte* buffer, csmSizeInt size);

    /**
     * Destructor
     */
    virtual ~CubismModelUserDataJson();

    /**
     * Returns the number of user data entries in the user data file.
     *
     * @return Number of user data entries
     */
    csmInt32 GetUserDataCount() const;

    /**
     * Returns the number of bytes in the user data file.
     *
     * @return Number of bytes
     */
    csmInt32 GetTotalUserDataSize() const;

    /**
     * Returns the type of user data at the specified index.
     *
     * @param i Index of the user data
     *
     * @return Type of user data
     */
    csmString GetUserDataTargetType(csmInt32 i) const;

    /**
     * Returns the ID of the target attached to the user data at the specified index.
     *
     * @param i Index of the user data
     *
     * @return ID of the target attached to the user data
     */
    CubismIdHandle GetUserDataId(csmInt32 i) const;

    /**
     * Returns the value of the user data at the specified index.
     *
     * @param i Index of the user data
     *
     * @return Value of the user data
     */
    const csmChar* GetUserDataValue(csmInt32 i) const;
};

}}}
