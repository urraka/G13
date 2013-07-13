#pragma once

#define IOS_ORIENTATION_PORTRAIT				0
#define IOS_ORIENTATION_PORTRAIT_UPSIDE_DOWN 	1
#define IOS_ORIENTATION_LANDSCAPE_LEFT			2
#define IOS_ORIENTATION_LANDSCAPE_RIGHT			3

typedef void ( *IOSAppLaunchedFun )();
typedef void ( *IOSTerminateFun   )();
typedef void ( *IOSDisplayFun     )();
typedef void ( *IOSOrientationFun )();

int  iosRun(int argc, char *argv[]);
void iosCreateWindow();
void iosGetWindowSize(int *width, int *height);
int  iosGetCurrentOrientation();

void iosSetAppLaunchedCallback (IOSAppLaunchedFun callback);
void iosSetTerminateCallback   (IOSTerminateFun   callback);
void iosSetDisplayCallback     (IOSDisplayFun     callback);
void iosSetOrientationCallback (IOSOrientationFun callback);
