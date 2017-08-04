#pragma once

#include "Anim.h"
#include "Material.h"
#include "Mesh.h"
#include "Program.h"
#include "RenderThread.h"
#include "Texture.h"

namespace ren {
    class Context : public RenderThread {
    protected:
        int w_ = 0, h_ = 0;

        MeshStorage		 meshes_;
        MaterialStorage	 materials_;
        ProgramStorage	 programs_;
        Texture2DStorage textures_;
        AnimSeqStorage   anims_;

#if defined(USE_GL_RENDER)
        std::string glsl_defines_;
#endif
    public:
        ~Context();

        void Init(int w, int h);

        int w() const { return w_; }
        int h() const { return h_; }

        void Resize(int w, int h);

        /*** Mesh ***/
        MeshRef LoadMesh(const char *name, void *data, material_load_callback on_mat_load);

        /*** Material ***/
        MaterialRef LoadMaterial(const char *name, const char *mat_src, eMatLoadStatus *status, const program_load_callback &on_prog_load,
                                 const texture_load_callback &on_tex_load);
        int NumMaterialsNotReady();
        void ReleaseMaterials();

        /*** Program ***/
#if defined(USE_GL_RENDER)
        ProgramRef LoadProgramGLSL(const char *name, const char *vs_source, const char *fs_source, eProgLoadStatus *load_status);
#elif defined(USE_SW_RENDER)
        ProgramRef LoadProgramSW(const char *name, void *vs_shader, void *fs_shader, int num_fvars,
                                 const Attribute *attrs, const Uniform *unifs, eProgLoadStatus *load_status);
#endif
        int NumProgramsNotReady();
        void ReleasePrograms();

        /*** Texture ***/
        Texture2DRef LoadTexture2D(const char *name, const void *data, int size, const Texture2DParams &p, eTexLoadStatus *load_status);
        Texture2DRef LoadTextureCube(const char *name, const void *data[6], const int size[6], const Texture2DParams &p, eTexLoadStatus *load_status);

        int NumTexturesNotReady();
        void ReleaseTextures();

        /*** Anims ***/
        AnimSeqRef LoadAnimSequence(const char *name, void *data);
        int NumAnimsNotReady();
        void ReleaseAnims();

#if defined(USE_GL_RENDER)
        float anisotropy = 0;
        int max_uniform_vec4 = 0;
        bool IsExtensionSupported(const char *ext);
#elif defined(USE_SW_RENDER)
        int max_uniform_vec4 = 0;
#endif
    };

#if defined(USE_GL_RENDER)
    void CheckError(const char *op);
#endif
}