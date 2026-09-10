/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#import "axmol/ui/EditBox/Mac/TextFieldFormatter.h"

@implementation AxmolTextFieldFormatter {
}

@synthesize maximumLength = _maximumLength;

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _maximumLength = INT_MAX;
    }

    return self;
}

- (NSString*)stringForObjectValue:(id)object
{
    return object;
}

- (BOOL)getObjectValue:(id*)object forString:(NSString*)string errorDescription:(NSString**)error
{
    *object = string;
    return YES;
}

- (BOOL)isPartialStringValid:(NSString**)partialStringPtr
       proposedSelectedRange:(NSRangePointer)proposedSelRangePtr
              originalString:(NSString*)origString
       originalSelectedRange:(NSRange)origSelRange
            errorDescription:(NSString**)error
{
    return (*partialStringPtr).length <= self.maximumLength;
}

- (NSAttributedString*)attributedStringForObjectValue:(id)anObject withDefaultAttributes:(NSDictionary*)attributes
{
    return nil;
}
@end
