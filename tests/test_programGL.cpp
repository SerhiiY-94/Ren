#include "test_common.h"

#include <SDL2/SDL.h>

#include "../GL.h"
#include "../Context.h"
#include "../Material.h"

class ProgramTest : public ren::Context {
    SDL_Window *window_;
    void *gl_ctx_;
public:
    ProgramTest() {
        SDL_Init(SDL_INIT_VIDEO);

        window_ = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        gl_ctx_ = SDL_GL_CreateContext(window_);

        Context::Init(256, 256);
    }

    ~ProgramTest() {
        SDL_GL_DeleteContext(gl_ctx_);
        SDL_DestroyWindow(window_);
#ifndef EMSCRIPTEN
        SDL_Quit();
#endif
    }
};

void test_program() {
    {
        // Load program
        ProgramTest test;

        const char vs_src[] =	\
                                " \
            /*\n \
            ATTRIBUTES\n \
                aVertexPosition : 0\n \
                aVertexPosition1 : 1\n \
            UNIFORMS\n \
                uMVPMatrix : 0\n \
            */\n \
            \n \
            attribute vec3 aVertexPosition;\n \
            uniform mat4 uMVPMatrix;\n \
            \n \
            void main(void) {\n \
                gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);\n \
            } ";

        const char fs_src[] =
            " \
            precision mediump float;\n \
            /*\n \
            UNIFORMS\n \
                asdasd : 1\n \
            */\n \
            uniform vec3 col;\n \
            \n\
            void main(void) {\n \
                gl_FragColor = vec4(col, 1.0);\n \
            }";

        ren::eProgLoadStatus status;
        ren::ProgramRef p = test.LoadProgramGLSL("constant", nullptr, nullptr, &status);

        assert(status == ren::ProgSetToDefault);
        assert(std::string(p->name()) == "constant");
        assert(p->prog_id() == 0); // not initialized
        assert(p->ready() == false);

        test.LoadProgramGLSL("constant", vs_src, fs_src, &status);

        assert(status == ren::ProgCreatedFromData);

        assert(std::string(p->name()) == "constant");

        assert(p->ready() == true);

        assert(p->attribute(0).name == "aVertexPosition");
        assert(p->attribute(0).loc != -1);
        assert(p->attribute(1).name.empty());
        assert(p->attribute(1).loc == -1);

        assert(p->uniform(0).name == "uMVPMatrix");
        assert(p->uniform(0).loc != -1);
        assert(p->uniform(1).name == "col");
        assert(p->uniform(1).loc != -1);
    }
}
