/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#import "axmol/platform/ios/AxmolViewController.h"
#import "axmol/platform/ios/RenderHostView-ios.h"
#import "axmol/platform/ios/RenderView-ios.h"
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
- (void)loadView
{
    // create platform render view
    auto r                   = [[UIScreen mainScreen] bounds];
    RenderHostView* hostView = [RenderHostView viewWithFrame:r
                                                 pixelFormat:(int)RenderView::_pixelFormat
                                                 depthFormat:(int)RenderView::_depthFormat
                                          preserveBackbuffer:NO
                                                  sharegroup:nil
                                               multiSampling:RenderView::_multisamplingCount > 0 ? YES : NO
                                             numberOfSamples:RenderView::_multisamplingCount];

    // Not available on tvOS
#if !defined(AX_TARGET_OS_TVOS)
    [hostView setMultipleTouchEnabled:YES];
#endif
    self.view = hostView;
}

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

    switch (preferred)
    {
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
