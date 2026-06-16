/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/csmString.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismIdManager;

/**
 * Handles the ID of parameters, parts, and drawing objects.
 */
struct CubismId
{
    friend class CubismIdManager;

    /**
     * Returns the ID.
     *
     * @return ID
     */
    const csmString& GetString() const;

    /**
     * Assigns the ID held by another CubismId to this ID.
     *
     * @param c CubismId object to assign
     *
     * @return Reference to this object after assignment
     */
    CubismId& operator=(const CubismId& c);

    /**
     * Compares if the ID held by another CubismId is equal to the ID held by this object.
     *
     * @param c CubismId object to compare
     *
     * @return true if the IDs are equal; otherwise false.
     */
    csmBool operator==(const CubismId& c) const;

    /**
     * Compares if the ID held by another CubismId is not equal to the ID held by this object.
     *
     * @param c CubismId object to compare
     *
     * @return true if the IDs are not equal; otherwise false.
     */
    csmBool operator!=(const CubismId& c) const;

private:
    CubismId();

    CubismId(const csmChar* id);

    ~CubismId();

    CubismId(const CubismId& c);

    csmString _id;
};

typedef const CubismId* CubismIdHandle;

}}}
