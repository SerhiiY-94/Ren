#include "test_common.h"

#include <SDL2/SDL.h>

#include "../Context.h"
#include "../Texture.h"

#ifdef USE_GL_RENDER

#include "../GL.h"

class TextureTest : public ren::Context {
	SDL_Window *window_;
	void *gl_ctx_;
public:
	TextureTest() {
		SDL_Init(SDL_INIT_VIDEO);

		window_ = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
		gl_ctx_ = SDL_GL_CreateContext(window_);
#ifndef EMSCRIPTEN
		GLenum glew_err = glewInit();
		if (glew_err != GLEW_OK) {
			fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));
		}
#endif
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

class TextureTest {
    SWcontext *ctx;
public:
    TextureTest() {
        ctx = swCreateContext(1, 1);
        swMakeCurrent(ctx);
    }

    ~TextureTest() {
        swDeleteContext(ctx);
    }
};
#endif

static unsigned char test_tga_img[] =	"\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x02\x00" \
										"\x18\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00" \
										"\x00\x00\x00\x00\x00\x00\x54\x52\x55\x45\x56\x49\x53\x49\x4F\x4E" \
										"\x2D\x58\x46\x49\x4C\x45\x2E\x00";

void test_texture() {
    {   // TGA load
        TextureTest test;

        ren::eTexLoadStatus status;
		ren::Texture2DParams p;
		ren::Texture2DRef t_ref = test.LoadTexture2D("checker.tga", nullptr, 0, p, &status);
        assert(status == ren::TexCreatedDefault);

		assert(std::string(t_ref->name()) == "checker.tga");
		assert(t_ref->params().w == 1);
		assert(t_ref->params().h == 1);
		assert(t_ref->params().format == ren::RawRGB888);
		assert(t_ref->ready() == false);

        {
            ren::Texture2DRef t_ref2 = test.LoadTexture2D("checker.tga", nullptr, 0, p, &status);
            assert(status == ren::TexFound);
			assert(t_ref2->ready() == false);
        }

        {
            ren::Texture2DRef t_ref3 = test.LoadTexture2D("checker.tga", test_tga_img, (int)sizeof(test_tga_img), p, &status);
            assert(status == ren::TexCreatedFromData);
			assert(t_ref3->params().w == 2);
			assert(t_ref3->params().h == 2);
			assert(t_ref3->params().format == ren::RawRGB888);
			assert(t_ref3->ready() == true);
        }
    }
}
