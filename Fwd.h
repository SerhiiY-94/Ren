#pragma once

#include <math/math.hpp>

#include "Storage.h"

namespace ren {
    class Anim;
    class Camera;
    class Material;
    class Mesh;
    class Program;
    class Texture2D;

    template <typename val_t>
    using mat_container = std::vector<val_t, math::aligned_allocator<val_t, math::vec4::alignment>>;

    template <typename val_t>
    using mesh_container = std::vector<val_t, math::aligned_allocator<val_t, math::vec3::alignment>>;

    typedef StorageRef<Anim> AnimRef;
    typedef StorageRef<Material, mat_container> MaterialRef;
    typedef StorageRef<Mesh, mesh_container> MeshRef;
    typedef StorageRef<Program> ProgramRef;
    typedef StorageRef<Texture2D> Texture2DRef;

#if defined(USE_GL_RENDER)
    void CheckError(const char *op = "undefined");
#endif
}