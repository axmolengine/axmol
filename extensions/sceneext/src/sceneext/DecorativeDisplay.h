/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ArmatureDefine.h"
#include "sceneext/DisplayFactory.h"
#include "sceneext/AnimationData.h"
#include "sceneext/SceneExtMacros.h"

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
#    include "sceneext/ColliderDetector.h"
#endif

namespace ax
{
class Node;
}

namespace ax::ext
{
/**
 *  @lua NA
 */
class SCNEXT_API DecorativeDisplay : public ax::Object
{
public:
    static DecorativeDisplay* create();

public:
    DecorativeDisplay(void);
    ~DecorativeDisplay(void);

    virtual bool init();

    virtual void setDisplay(ax::Node* display);
    virtual ax::Node* getDisplay() const { return _display; }

    virtual void setDisplayData(DisplayData* data)
    {
        if (_displayData != data)
        {
            AX_SAFE_RETAIN(data);
            AX_SAFE_RELEASE(_displayData);
            _displayData = data;
        }
    }
    virtual DisplayData* getDisplayData() const { return _displayData; }

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    virtual void setColliderDetector(ColliderDetector* detector)
    {
        if (_colliderDetector != detector)
        {
            AX_SAFE_RETAIN(detector);
            AX_SAFE_RELEASE(_colliderDetector);
            _colliderDetector = detector;
        }
    }
    virtual ColliderDetector* getColliderDetector() const { return _colliderDetector; }
#endif
protected:
    ax::Node* _display;
    DisplayData* _displayData;

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    ColliderDetector* _colliderDetector;
#endif
};

}  // namespace ax::ext
