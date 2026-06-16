/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "ICubismUpdater.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

csmBool ICubismUpdater::SortFunction(ICubismUpdater* left, ICubismUpdater* right)
{
    return left->_executionOrder < right->_executionOrder;
}

ICubismUpdater::ICubismUpdater()
    : _executionOrder(CubismUpdateOrder_Max)
{
}

ICubismUpdater::ICubismUpdater(const csmInt32 executionOrder)
    : _executionOrder(executionOrder)
{
}
}}}
