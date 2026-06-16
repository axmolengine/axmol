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
 * Interface class for providing parameter values.<br>
 * Defines the base interface for classes that supply parameter values to the model.
 */
class IParameterProvider
{
public:
    /**
     * Constructor
     */
    IParameterProvider() = default;

    /**
     * Destructor
     */
    virtual ~IParameterProvider() = default;

    /**
     * Update process.
     *
     * @return  true if the update is successful.
     */
    virtual csmBool Update();

    /**
     * Update process.
     *
     * @param deltaTimeSeconds Delta time in seconds.
     *
     * @return  true if the update is successful.
     */
    virtual csmBool Update(csmFloat32 deltaTimeSeconds);

    /**
     * Retrieves the current value of the parameter.
     *
     * @return  The parameter value as a floating-point number..
     */
    virtual csmFloat32 GetParameter() = 0;
};

}}}
