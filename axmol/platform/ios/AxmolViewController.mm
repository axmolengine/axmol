/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

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

#import "axmol/platform/ios/AxmolViewController.h"
#import "axmol/platform/ios/EARenderView-ios.h"
#include "axmol/platform/Device.h"
#include "axmol/platform/Application.h"
#include "axmol/base/Director.h"

using namespace ax;

@implementation AxmolViewController

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform
customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

// Implement loadView to create a view hierarchy programmatically, without using a nib.
// - (void)loadView
// {
// }

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

- (NSUInteger)supportedInterfaceOrientations
{
    const auto preferred = Device::getPreferredOrientation();

    switch (preferred) {
        case Device::Orientation::Portrait:
            return UIInterfaceOrientationMaskPortrait;
        case Device::Orientation::ReversePortrait:
            return UIInterfaceOrientationMaskPortraitUpsideDown;
        case Device::Orientation::Landscape:
            return UIInterfaceOrientationMaskLandscapeLeft;
        case Device::Orientation::ReverseLandscape:
            return UIInterfaceOrientationMaskLandscapeRight;
        case Device::Orientation::SensorLandscape:
            return UIInterfaceOrientationMaskLandscape;
        case Device::Orientation::SensorPortrait:
            return UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown;
        case Device::Orientation::Sensor:
            return UIInterfaceOrientationMaskAllButUpsideDown;
        case Device::Orientation::FullSensor:
            return UIInterfaceOrientationMaskAll;
        default:
            break;
    }
    return UIInterfaceOrientationUnknown;
}

- (BOOL)shouldAutorotate
{
    return YES;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [super didRotateFromInterfaceOrientation:fromInterfaceOrientation];
}

// fix not hide status on ios7
- (BOOL)prefersStatusBarHidden
{
    return YES;
}

// Controls the application's preferred home indicator auto-hiding when this view controller is shown.
- (BOOL)prefersHomeIndicatorAutoHidden
{
    return YES;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];

    // Release any cached data, images, etc that aren't in use.
}

@end
