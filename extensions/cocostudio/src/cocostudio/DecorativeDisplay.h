/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

https://axmolengine.github.io/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCDECORATIVEDISPLAY_H__
#define __CCDECORATIVEDISPLAY_H__

#include "ArmatureDefine.h"
#include "DisplayFactory.h"
#include "Datas.h"
#include "CocosStudioExport.h"

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_CHIPMUNK_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
#    include "ColliderDetector.h"
#endif

NS_AX_BEGIN
class Node;
NS_AX_END

namespace cocostudio
{
/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL DecorativeDisplay : public ax::Object
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

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_CHIPMUNK_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
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

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_CHIPMUNK_DETECT || ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    ColliderDetector* _colliderDetector;
#endif
};

}  // namespace cocostudio

#endif /*__CCDECORATIVEDISPLAY_H__*/
