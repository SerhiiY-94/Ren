
#include "Anim.cpp"
#include "Camera.cpp"
#include "Context.cpp"
#include "Material.cpp"
#include "Mesh.cpp"
#include "RenderThread.cpp"
#include "Utils.cpp"

extern "C" {
#include "SW/_SW.c"
}

#if defined(USE_GL_RENDER)
#if defined(__ANDROID__) || defined(__native_client__) || defined(EMSCRIPTEN)
#else
#include "GLExt.cpp"
#endif
#include "ContextGL.cpp"
#include "ProgramGL.cpp"
#include "TextureGL.cpp"
#elif defined(USE_SW_RENDER)
#include "ContextSW.cpp"
#include "ProgramSW.cpp"
#include "TextureSW.cpp"
#endif
