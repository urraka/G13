#include "../System/platform.h"
#if defined(IOS)
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
#else
	#include <GL/glew.h>
#endif
