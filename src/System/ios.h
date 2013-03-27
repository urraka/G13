#pragma once

typedef void (*IOSAppLaunchedFun)();
typedef void (*IOSTerminateFun)();
typedef void (*IOSDisplayFun)();
typedef void (*IOSOrientationFun)();

int iosRun(int argc, char *argv[]);
void iosCreateWindow();
void iosGetWindowSize(int *width, int *height);
void iosGetCurrentOrientation();
void iosSetAppLaunchedCallback(IOSAppLaunchedFun callback);
void iosSetTerminateCallback(IOSTerminateFun callback);
void iosSetDisplayCallback(IOSDisplayFun callback);
void iosSetOrientationCallback(IOSOrientationFun callback);
