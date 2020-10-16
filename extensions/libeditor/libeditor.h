/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

http://www.cocos2d-x.org

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

#ifndef __EDITOR_SUPPORT_COCOSTUDIO_H__
#define __EDITOR_SUPPORT_COCOSTUDIO_H__

#include "libeditor/CCActionFrame.h"
#include "libeditor/CCActionFrameEasing.h"
#include "libeditor/CCActionManagerEx.h"
#include "libeditor/CCActionNode.h"
#include "libeditor/CCActionObject.h"
#include "libeditor/CCArmature.h"
#include "libeditor/CCBone.h"
#include "libeditor/CCArmatureAnimation.h"
#include "libeditor/CCProcessBase.h"
#include "libeditor/CCTween.h"
#include "libeditor/CCDatas.h"
#include "libeditor/CCBatchNode.h"
#include "libeditor/CCDecorativeDisplay.h"
#include "libeditor/CCDisplayFactory.h"
#include "libeditor/CCDisplayManager.h"
#include "libeditor/CCSkin.h"
#include "libeditor/CCColliderDetector.h"
#include "libeditor/CCArmatureDataManager.h"
#include "libeditor/CCArmatureDefine.h"
#include "libeditor/CCDataReaderHelper.h"
#include "libeditor/CCTransformHelp.h"
#include "libeditor/CCUtilMath.h"
#include "libeditor/CCComBase.h"
#include "libeditor/CCComAttribute.h"
#include "libeditor/CCComAudio.h"
#include "libeditor/CCComController.h"
#include "libeditor/CCComRender.h"
#include "libeditor/CCInputDelegate.h"
#include "libeditor/DictionaryHelper.h"
#include "libeditor/CCSGUIReader.h"
#include "libeditor/CCSSceneReader.h"
#include "libeditor/TriggerBase.h"
// #include "libeditor/ActionTimeline/CCActionTimelineCache.h"
#include "libeditor/ActionTimeline/CCFrame.h"
#include "libeditor/ActionTimeline/CCTimeLine.h"
#include "libeditor/ActionTimeline/CCActionTimeline.h"
#include "libeditor/ActionTimeline/CCActionTimelineNode.h"
#include "libeditor/ActionTimeline/CCBoneNode.h"
#include "libeditor/ActionTimeline/CCSkeletonNode.h"
// #include "libeditor/ActionTimeline/CSLoader.h"
#include "libeditor/CocosStudioExport.h"

namespace cocostudio
{
    void CCS_DLL destroyCocosStudio();
}

#endif
