#include "platform.h"
#include "ios.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <assert.h>

// -----------------------------------------------------------------------------
// GLView interface
// -----------------------------------------------------------------------------

@interface GLView : UIView {
		CAEAGLLayer* layer_;
		EAGLContext* context_;
	}
@end

// -----------------------------------------------------------------------------
// AppDelegate interface
// -----------------------------------------------------------------------------

@interface AppDelegate : NSObject<UIApplicationDelegate> {
		UIWindow *window_;
		GLView *view_;
	}

	- (void)createWindow;
@end

// -----------------------------------------------------------------------------
// Module locals
// -----------------------------------------------------------------------------

namespace {

AppDelegate *app = 0;
GLView *view = 0;

IOSAppLaunchedFun appLaunchedCallback = 0;
IOSTerminateFun terminateCallback = 0;
IOSDisplayFun displayCallback = 0;
IOSOrientationFun orientationCallback = 0;

int currentOrientation = IOS_ORIENTATION_PORTRAIT;

void updateOrientation()
{
	UIInterfaceOrientation statusBarOrientation = [[UIApplication sharedApplication] statusBarOrientation];

	switch ([UIDevice currentDevice].orientation)
	{
		case UIDeviceOrientationPortrait:
			currentOrientation = IOS_ORIENTATION_PORTRAIT;
			statusBarOrientation = UIInterfaceOrientationPortrait;
			break;

		case UIDeviceOrientationPortraitUpsideDown:
			currentOrientation = IOS_ORIENTATION_PORTRAIT_UPSIDE_DOWN;
			statusBarOrientation = UIInterfaceOrientationPortraitUpsideDown;
			break;

		case UIDeviceOrientationLandscapeLeft:
			currentOrientation = IOS_ORIENTATION_LANDSCAPE_LEFT;
			statusBarOrientation = UIInterfaceOrientationLandscapeRight;
			break;

		case UIDeviceOrientationLandscapeRight:
			currentOrientation = IOS_ORIENTATION_LANDSCAPE_RIGHT;
			statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
			break;

		default:
			break;
	}

	[[UIApplication sharedApplication] setStatusBarOrientation:statusBarOrientation];
}

} // unnamed

// -----------------------------------------------------------------------------
// GLView implementation
// -----------------------------------------------------------------------------

@implementation GLView
	+ (Class)layerClass
	{
		return [CAEAGLLayer class];
	}

	- (id)initWithFrame:(CGRect)frame
	{
		self = [super initWithFrame:frame];

		if (!self)
			return nil;

		layer_ = (CAEAGLLayer*)self.layer;
		layer_.opaque = YES;

		EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
		context_ = [[EAGLContext alloc] initWithAPI:api];

		if (!context_) {
			NSLog(@"Failed to initialize OpenGLES 2.0 context.");
			exit(1);
		}

		if (![EAGLContext setCurrentContext:context_]) {
			NSLog(@"Failed to set current OpenGL context.");
			exit(1);
		}

		GLuint renderBuffer;
		glGenRenderbuffers(1, &renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		[context_ renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer_];

		GLuint framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);

		CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(display:)];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

		return self;
	}

	- (void)dealloc
	{
		if (terminateCallback != 0)
			terminateCallback();

		[context_ release];
		context_ = nil;
		[super dealloc];
	}

	- (void)display:(CADisplayLink*)displayLink
	{
		displayCallback();

		[context_ presentRenderbuffer:GL_RENDERBUFFER];
	}
@end

// -----------------------------------------------------------------------------
// AppDelegate implementation
// -----------------------------------------------------------------------------

@implementation AppDelegate
	- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
	{
		app = self;

		[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];
		[[UIApplication sharedApplication] setStatusBarHidden:true];
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:UIDeviceOrientationDidChangeNotification object:nil];

		updateOrientation();
		appLaunchedCallback();

		assert(displayCallback != 0);

		return YES;
	}

	- (void)dealloc
	{
		[view_ release];
		[window_ release];
		[super dealloc];
	}

	- (void)applicationWillResignActive:(UIApplication *)application {}
	- (void)applicationDidEnterBackground:(UIApplication *)application {}
	- (void)applicationWillEnterForeground:(UIApplication *)application {}
	- (void)applicationDidBecomeActive:(UIApplication *)application {}

	- (void)applicationWillTerminate:(UIApplication *)application
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self];
		[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
	}

	- (void)orientationChanged: (NSNotification*) notice
	{
		updateOrientation();

		if (orientationCallback)
			orientationCallback();
	}

	- (void)createWindow
	{
		CGRect screen = [[UIScreen mainScreen] bounds];
		window_ = [[UIWindow alloc] initWithFrame: screen];
		view = view_ = [[GLView alloc] initWithFrame: screen];
		[window_ addSubview: view_];
		[window_ makeKeyAndVisible];
	}
@end

// -----------------------------------------------------------------------------
// C-like bindings
// -----------------------------------------------------------------------------

void iosSetAppLaunchedCallback(IOSAppLaunchedFun callback)
{
	appLaunchedCallback = callback;
}

void iosSetTerminateCallback(IOSTerminateFun callback)
{
	terminateCallback = callback;
}

void iosSetDisplayCallback(IOSDisplayFun callback)
{
	displayCallback = callback;
}

void iosSetOrientationCallback(IOSOrientationFun callback)
{
	orientationCallback = callback;
}

int iosRun(int argc, char *argv[])
{
	assert(appLaunchedCallback != 0);

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	[pool release];

	return retVal;
}

void iosCreateWindow()
{
	assert(app != 0);
	[app createWindow];
}

void iosGetWindowSize(int *width, int *height)
{
	assert(view != 0);
	*width = view.frame.size.width;
	*height = view.frame.size.height;
}

int iosGetCurrentOrientation()
{
	return currentOrientation;
}
