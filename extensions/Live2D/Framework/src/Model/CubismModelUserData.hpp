/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismModel.hpp"

namespace Live2D {  namespace Cubism {  namespace Framework {

typedef CubismIdHandle  ModelUserDataType;

/**
 * Handles management of user data.
 */
class CubismModelUserData
{
public:
    /**
     * Structure for user data information
     */
    struct CubismModelUserDataNode
    {
        ModelUserDataType   TargetType;         ///< User data type
        CubismIdHandle      TargetId;           ///< ID of the object attached to the user data
        csmString           Value;              ///< User data value
    };

    /**
     * Makes an instance.
     *
     * @param buffer Buffer where the user data file is loaded
     * @param size Byte size of the buffer
     *
     * @return Instance
     */
    static CubismModelUserData* Create(const csmByte* buffer, csmSizeInt size);

    /**
     * Destroys the instance.
     *
     * @param modelUserData Instance of `CubismModelUserData` to destroy
     */
    static void Delete(CubismModelUserData* modelUserData);

    /**
     * Destructor
     */
    virtual ~CubismModelUserData();

    /**
     * Returns the list of user data for ArtMesh.
     *
     * @return List of user data
     */
    const csmVector<const CubismModelUserDataNode*>& GetArtMeshUserDatas() const;

private:

    void ParseUserData(const csmByte* buffer, csmSizeInt size);

    csmVector<const CubismModelUserDataNode*>    _userDataNodes;
    csmVector<const CubismModelUserDataNode*>    _artMeshUserDataNodes;
};
}}} //--------- LIVE2D NAMESPACE ------------
