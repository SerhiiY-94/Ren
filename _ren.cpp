
#include "Anim.cpp"
#include "Camera.cpp"
#include "Context.cpp"
#include "Material.cpp"
#include "Matrices_old.cpp"
#include "Mesh.cpp"
#include "RenderThread.cpp"
#include "Utils.cpp"

extern "C" {
#include "SW/_SW.c"
}

#if defined(USE_GL_RENDER)
#include "ContextGL.cpp"
#include "ProgramGL.cpp"
#include "TextureGL.cpp"
#elif defined(USE_SW_RENDER)
#include "TextureSW.cpp"
#endif
