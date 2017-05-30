#pragma once

#if defined(USE_GL_RENDER)
#include "TextureGL.h"
#elif defined(USE_SW_RENDER)
#include "TextureSW.h"
#endif

namespace ren {
    class Context;
}
