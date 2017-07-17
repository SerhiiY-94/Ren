#pragma once

#include <cstdint>

#include <functional>

#include <math/math.hpp>

#include "Program.h"
#include "Storage.h"
#include "Texture.h"

namespace ren {
    enum eMaterialFlags { AlphaBlend = 1, DoubleSided = 2 };

    enum eMatLoadStatus { MatFound, MatSetToDefault, MatCreatedFromData };

    typedef std::function<Texture2DRef(const char *name)> texture_load_callback;
    typedef std::function<ProgramRef(const char *name, const char *arg1, const char *arg2)> program_load_callback;

    class Material : public RefCounter {
        uint32_t		flags_ = 0;
        bool			ready_ = false;
        char			name_[32];
        ProgramRef		program_;
        Texture2DRef	textures_[4];
        math::vec4		params_[8];

        void InitFromTXT(const char *name, const char *mat_src, eMatLoadStatus *status, const program_load_callback &on_prog_load,
                         const texture_load_callback &on_tex_load);
    public:
        Material() { name_[0] = '\0'; }
        Material(const char *name, const char *mat_src, eMatLoadStatus *status, const program_load_callback &on_prog_load,
                 const texture_load_callback &on_tex_load);

        uint32_t flags() const { return flags_; }
        bool ready() const { return ready_; }
        const char *name() const { return name_; }
        const ProgramRef &program() const { return program_; }
        const Texture2DRef &texture(int i) const { return textures_[i]; }
        const math::vec4 &param(int i) const { return params_[i]; }

        void Init(const char *name, const char *mat_src, eMatLoadStatus *status, const program_load_callback &on_prog_load,
                  const texture_load_callback &on_tex_load);
    };

    template <typename val_t>
    using aligned_container = std::vector<val_t, math::aligned_allocator<val_t, math::vec4::alignment>>;

    typedef StorageRef<Material, aligned_container> MaterialRef;
    typedef Storage<Material, aligned_container> MaterialStorage;
}