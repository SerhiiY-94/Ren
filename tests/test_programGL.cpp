#include "test_common.h"

#include "../GL.h"
#include "../Context.h"
#include "../Material.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

class ProgramTest : public Ren::Context {
#if defined(_WIN32)
    HINSTANCE hInstance;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
#else
    SDL_Window *window_;
    void *gl_ctx_;
#endif
public:
    ProgramTest() {
#if defined(_WIN32)
        hInstance = GetModuleHandle(NULL);
        WNDCLASS wc;
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = ::DefWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "ProgramTest";

        if (!RegisterClass(&wc)) {
            throw std::runtime_error("Cannot register window class!");
        }

        hWnd = CreateWindow("ProgramTest", "!!", WS_OVERLAPPEDWINDOW |
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                            0, 0, 100, 100, NULL, NULL, hInstance, NULL);

        if (hWnd == NULL) {
            throw std::runtime_error("Cannot create window!");
        }

        hDC = GetDC(hWnd);

        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(pfd));
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        int pf = ChoosePixelFormat(hDC, &pfd);
        if (pf == 0) {
            throw std::runtime_error("Cannot find pixel format!");
        }

        if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
            throw std::runtime_error("Cannot set pixel format!");
        }

        DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);
#else
        SDL_Init(SDL_INIT_VIDEO);

        window_ = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        gl_ctx_ = SDL_GL_CreateContext(window_);
#endif
        Context::Init(256, 256);
    }

    ~ProgramTest() {
#if defined(_WIN32)
        wglMakeCurrent(NULL, NULL);
        ReleaseDC(hWnd, hDC);
        wglDeleteContext(hRC);
        DestroyWindow(hWnd);
        UnregisterClass("ProgramTest", hInstance);
#else
        SDL_GL_DeleteContext(gl_ctx_);
        SDL_DestroyWindow(window_);
#ifndef EMSCRIPTEN
        SDL_Quit();
#endif
#endif
    }
};

void test_program() {
    {
        // Load program
        ProgramTest test;

        const char vs_src[] =   \
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
            #ifdef GL_ES\n \
				precision mediump float;\n \
			#else\n \
				#define lowp\n \
				#define mediump\n \
				#define highp\n \
			#endif\n \
            /*\n \
            UNIFORMS\n \
                asdasd : 1\n \
            */\n \
            uniform vec3 col;\n \
            \n\
            void main(void) {\n \
                gl_FragColor = vec4(col, 1.0);\n \
            }";

        Ren::eProgLoadStatus status;
        Ren::ProgramRef p = test.LoadProgramGLSL("constant", nullptr, nullptr, &status);

        assert(status == Ren::ProgSetToDefault);
        assert(std::string(p->name()) == "constant");
        assert(p->prog_id() == 0); // not initialized
        assert(p->ready() == false);

        test.LoadProgramGLSL("constant", vs_src, fs_src, &status);

        assert(status == Ren::ProgCreatedFromData);

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
