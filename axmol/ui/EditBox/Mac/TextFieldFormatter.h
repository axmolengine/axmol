/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <Foundation/Foundation.h>

@interface AxmolTextFieldFormatter : NSFormatter {
    int _maximumLength;
}

@property(nonatomic, assign) int maximumLength;

@end
