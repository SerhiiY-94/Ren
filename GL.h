#pragma once

#if defined(__ANDROID__) || defined(__native_client__) || defined(EMSCRIPTEN)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/glew.h>
#endif

