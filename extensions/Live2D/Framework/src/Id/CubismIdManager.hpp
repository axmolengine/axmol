/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/CubismBasicType.hpp"
#include "Type/csmString.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

struct CubismId;

/**
 * Handles ID names.
 */
class CubismIdManager
{
    friend struct CubismId;

public:
    /**
     * Constructor
     */
    CubismIdManager();

    /**
     * Destructor
     */
    ~CubismIdManager();

    /**
     * Registers IDs.
     *
     * @param ids Array of ID strings
     * @param count Number of IDs
     */
    void RegisterIds(const csmChar** ids, csmInt32 count);

    /**
     * Registers IDs.
     *
     * @param ids Collection of ID strings
     */
    void RegisterIds(const csmVector<csmString>& ids);

    /**
     * Registers an ID.
     *
     * @param id ID string
     */
    const CubismId* RegisterId(const csmChar* id);

    /**
     * Registers an ID.
     *
     * @param id ID string
     *
     * @return Registered ID<br>
     * If the ID was already registered, the already registered ID
     */
    const CubismId* RegisterId(const csmString& id);

    /**
     * Returns an ID.
     *
     * @param id ID string
     *
     * @return ID
     *
     * @note If the ID is not registered, it registers the ID.
     */
    const CubismId* GetId(const csmString& id);

    /**
     * Returns an ID.
     *
     * @param id ID string
     *
     * @return ID
     *
     * @note If the requested ID is not registered, it registers the ID.
     */
    const CubismId* GetId(const csmChar* id);

    /**
     * Checks if an ID is registered.
     *
     * @param id ID string to check
     *
     * @return true if the ID is registered; otherwise false.
     */
    csmBool IsExist(const csmString& id) const;

    /**
     * Checks if an ID is registered.
     *
     * @param id ID string to check
     *
     * @return true if the ID is registered; otherwise false.
     */
    csmBool IsExist(const csmChar* id) const;

private:
    CubismIdManager(const CubismIdManager&);
    CubismIdManager& operator=(const CubismIdManager&);

    CubismId* FindId(const csmChar* id) const;

    csmVector<CubismId*> _ids;
};

}}}
