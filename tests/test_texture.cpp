#include "test_common.h"

#include <SDL2/SDL.h>

#include "../Context.h"
#include "../Texture.h"

#ifdef USE_GL_RENDER

#include "../GL.h"

class TextureTest : public Ren::Context {
    SDL_Window *window_;
    void *gl_ctx_;
public:
    TextureTest() {
        SDL_Init(SDL_INIT_VIDEO);

        window_ = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        gl_ctx_ = SDL_GL_CreateContext(window_);

        Context::Init(256, 256);
    }

    ~TextureTest() {
        SDL_GL_DeleteContext(gl_ctx_);
        SDL_DestroyWindow(window_);
#ifndef EMSCRIPTEN
        SDL_Quit();
#endif
    }
};
#else

#include "../SW/SW.h"

class TextureTest : public Ren::Context {
public:
    TextureTest() {
        Ren::Context::Init(1, 1);
    }
};
#endif

static unsigned char test_tga_img[] = "\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x02\x00" \
                                      "\x18\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00" \
                                      "\x00\x00\x00\x00\x00\x00\x54\x52\x55\x45\x56\x49\x53\x49\x4F\x4E" \
                                      "\x2D\x58\x46\x49\x4C\x45\x2E\x00";

void test_texture() {
    {
        // TGA load
        TextureTest test;

        Ren::eTexLoadStatus status;
        Ren::Texture2DParams p;
        Ren::Texture2DRef t_ref = test.LoadTexture2D("checker.tga", nullptr, 0, p, &status);
        require(status == Ren::TexCreatedDefault);

        require(std::string(t_ref->name()) == "checker.tga");
        require(t_ref->params().w == 1);
        require(t_ref->params().h == 1);
        require(t_ref->params().format == Ren::RawRGB888);
        require(!t_ref->ready());

        {
            Ren::Texture2DRef t_ref2 = test.LoadTexture2D("checker.tga", nullptr, 0, p, &status);
            require(status == Ren::TexFound);
            require(!t_ref2->ready());
        }

        {
            Ren::Texture2DRef t_ref3 = test.LoadTexture2D("checker.tga", test_tga_img, (int)sizeof(test_tga_img), p, &status);
            require(status == Ren::TexCreatedFromData);
            require(t_ref3->params().w == 2);
            require(t_ref3->params().h == 2);
            require(t_ref3->params().format == Ren::RawRGB888);
            require(t_ref3->ready());
        }
    }
}
