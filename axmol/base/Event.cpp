/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/base/Event.h"

namespace ax
{

Event::Event(Type type) : _type(type), _isStopped(false), _currentTarget(nullptr) {}

Event::~Event() {}

}  // namespace ax
