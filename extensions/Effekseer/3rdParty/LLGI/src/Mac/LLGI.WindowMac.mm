#include <TargetConditionals.h>

#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)

#import "LLGI.WindowMac.h"
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

@interface LLGIApplication : NSApplication
{
	NSArray* nibObjects;
}

@end

@implementation LLGIApplication

- (void)sendEvent:(NSEvent*)event
{
	[super sendEvent:event];
}

- (void)doNothing:(id)object
{
}

@end

@interface LLGIApplicationDelegate : NSObject
@end

@implementation LLGIApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
	return NSTerminateCancel;
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
	[NSApp stop:nil];

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
										location:NSMakePoint(0, 0)
								   modifierFlags:0
									   timestamp:0
									windowNumber:0
										 context:nil
										 subtype:0
										   data1:0
										   data2:0];
	[NSApp postEvent:event atStart:YES];
	[pool drain];
}

@end

namespace LLGI
{

struct Cocoa_Impl
{
	static void initialize()
	{
		if (NSApp)
			return;
		[LLGIApplication sharedApplication];

		[NSThread detachNewThreadSelector:@selector(doNothing:) toTarget:NSApp withObject:nil];

		[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

		NSMenu* menubar = [NSMenu new];
		[NSApp setMainMenu:menubar];

		id delegate = [[LLGIApplicationDelegate alloc] init];

		[NSApp setDelegate:delegate];

		[NSApp run];
	}
};

struct WindowMac_Impl
{
	NSWindow* window_ = nullptr;
	NSAutoreleasePool* pool_ = nullptr;

	WindowMac_Impl(const char* title, Vec2I windowSize)
	{
		int width = windowSize.X;
		int height = windowSize.Y;

		NSRect frame = NSMakeRect(0, 0, width, height);
		window_ = [[NSWindow alloc] initWithContentRect:frame
											  styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)
												backing:NSBackingStoreBuffered
												  defer:NO];

		window_.title = [NSString stringWithCString:title encoding:NSUTF8StringEncoding];
		window_.releasedWhenClosed = false;
		[window_ center];
		[window_ orderFrontRegardless];

		pool_ = [[NSAutoreleasePool alloc] init];
	}

	~WindowMac_Impl()
	{
		if (window_ != nullptr)
		{
			[window_ close];
			[window_ release];
			window_ = nullptr;
		}

		[pool_ drain];
	}

	void gc()
	{
		[pool_ drain];
		pool_ = [[NSAutoreleasePool alloc] init];
	}

	bool newFrame()
	{
		for (;;)
		{
			NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
												untilDate:[NSDate distantPast]
												   inMode:NSDefaultRunLoopMode
												  dequeue:YES];
			if (event == nil)
				break;

			[NSApp sendEvent:event];
		}

		gc();

		if (!window_.isVisible)
		{
			return false;
		}

		return true;
	}
};

bool WindowMac::Initialize(const char* title, const Vec2I& windowSize)
{
	Cocoa_Impl::initialize();
	impl_ = std::make_shared<WindowMac_Impl>(title, windowSize);
	windowSize_ = windowSize;
	return true;
}

bool WindowMac::DoEvent() { return impl_->newFrame(); }

void WindowMac::Terminate() { impl_.reset(); }

void* WindowMac::GetNSWindowAsVoidPtr() { return impl_->window_; }

bool WindowMac::OnNewFrame() { return DoEvent(); }

void* WindowMac::GetNativePtr(int32_t index) { return GetNSWindowAsVoidPtr(); }

Vec2I WindowMac::GetWindowSize() const { return windowSize_; }

Vec2I WindowMac::GetFrameBufferSize() const
{
	@autoreleasepool
	{
		NSRect contentRect = [impl_->window_.contentView frame];
		NSRect rect = [impl_->window_.contentView convertRectToBacking:contentRect];
		return Vec2I(rect.size.width, rect.size.height);
	}
}

} // namespace LLGI

#endif
