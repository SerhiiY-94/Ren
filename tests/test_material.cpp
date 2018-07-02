#include "test_common.h"

#include <SDL2/SDL.h>

#include "../Context.h"
#include "../Material.h"

#if defined(USE_GL_RENDER)
#include "../GL.h"
class MaterialTest : public ren::Context {
    SDL_Window *window_;
    void *gl_ctx_;
public:
    MaterialTest() {
        SDL_Init(SDL_INIT_VIDEO);

        window_ = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        gl_ctx_ = SDL_GL_CreateContext(window_);

        Context::Init(256, 256);
    }

    ~MaterialTest() {
        SDL_GL_DeleteContext(gl_ctx_);
        SDL_DestroyWindow(window_);
#ifndef EMSCRIPTEN
        SDL_Quit();
#endif
    }
};
#elif defined(USE_SW_RENDER)
#include "../SW/SW.h"
class MaterialTest : public ren::Context {
public:
    MaterialTest() {
        ren::Context::Init(1, 1);
    }
};
#endif

static ren::ProgramRef OnProgramNeeded(const char *name, const char *arg1, const char *arg2) {
    return {};
}

static ren::Texture2DRef OnTextureNeeded(const char *name) {
    return {};
}

void test_material() {
    {
        // Load material
        MaterialTest test;

        auto on_program_needed = [&test](const char *name, const char *arg1, const char *arg2) {
            ren::eProgLoadStatus status;
#if defined(USE_GL_RENDER)
            return test.LoadProgramGLSL(name, nullptr, nullptr, &status);
#elif defined(USE_SW_RENDER)
            ren::Attribute _attrs[] = { {} };
            ren::Uniform _unifs[] = { {} };
            return test.LoadProgramSW(name, nullptr, nullptr, 0, _attrs, _unifs, &status);
#endif
        };

        auto on_texture_needed = [&test](const char *name) {
            ren::eTexLoadStatus status;
            ren::Texture2DParams p;
            return test.LoadTexture2D(name, nullptr, 0, p, &status);
        };

        const char *mat_src = "gl_program: constant constant.vs constant.fs\n"
                              "sw_program: constant\n"
                              "flag: alpha_blend\n"
                              "texture: checker.tga\n"
                              "texture: checker.tga\n"
                              "texture: metal_01.tga\n"
                              "texture: checker.tga\n"
                              "param: 0 1 2 3\n"
                              "param: 0.5 1.2 11 15";

        ren::eMatLoadStatus status;
        ren::MaterialRef m_ref = test.LoadMaterial("mat1", nullptr, &status, on_program_needed, on_texture_needed);
        require(status == ren::MatSetToDefault);

        {
            require(m_ref->ready() == false);
        }

        test.LoadMaterial("mat1", mat_src, &status, on_program_needed, on_texture_needed);

        require(status == ren::MatCreatedFromData);
        require(m_ref->flags() & ren::AlphaBlend);
        require(m_ref->ready() == true);
        require(std::string(m_ref->name()) == "mat1");

        ren::ProgramRef p = m_ref->program();

        require(std::string(p->name()) == "constant");
        require(p->ready() == false);

        ren::Texture2DRef t0 = m_ref->texture(0);
        ren::Texture2DRef t1 = m_ref->texture(1);
        ren::Texture2DRef t2 = m_ref->texture(2);
        ren::Texture2DRef t3 = m_ref->texture(3);

        require(t0 == t1);
        require(t0 == t3);

        require(std::string(t0->name()) == "checker.tga");
        require(std::string(t1->name()) == "checker.tga");
        require(std::string(t2->name()) == "metal_01.tga");
        require(std::string(t3->name()) == "checker.tga");

        require(m_ref->param(0) == math::vec4(0, 1, 2, 3));
        require(m_ref->param(1) == math::vec4(0.5f, 1.2f, 11, 15));
    }
}
