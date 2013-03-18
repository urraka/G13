#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <Graphics/OpenGL.h>
#include <Graphics/Graphics.h>
#include <Game/Game.h>

// GLView interface

@interface GLView : UIView {
	CAEAGLLayer* layer_;
	EAGLContext* context_;
}
@end

// AppDelegate interface

@interface AppDelegate : NSObject<UIApplicationDelegate> {
	UIWindow *window_;
	GLView *view_;
}
@end

// GLView implementation

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

	CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(loop:)];
	[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

	game.resolution = ivec2(self.frame.size.height, self.frame.size.width);
	game.init();

	return self;
}

- (void)dealloc
{
	game.terminate();

	[context_ release];
	context_ = nil;
	[super dealloc];
}

- (void)loop:(CADisplayLink*)displayLink
{
	game.draw();
	game.input();
	game.update();

	[context_ presentRenderbuffer:GL_RENDERBUFFER];
}

@end

// AppDelegate implementation

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];

	CGRect screen = [[UIScreen mainScreen] bounds];
	window_ = [[UIWindow alloc] initWithFrame: screen];
	view_ = [[GLView alloc] initWithFrame: screen];
	[window_ addSubview: view_];
	[window_ makeKeyAndVisible];
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
- (void)applicationWillTerminate:(UIApplication *)application {}

@end

// Entry point

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	[pool release];
	return retVal;
}
