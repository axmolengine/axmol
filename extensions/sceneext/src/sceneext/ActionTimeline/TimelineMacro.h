/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#ifdef __cplusplus
#    define NS_TIMELINE_BEGIN \
        namespace ax::ext     \
        {                     \
        namespace timeline    \
        {
#    define NS_TIMELINE_END \
        }                   \
        }
#    define USING_NS_TIMELINE using namespace ax::ext::timeline;
#else
#    define NS_TIMELINE_BEGIN
#    define NS_TIMELINE_END
#    define USING_NS_TIMELINE
#endif
