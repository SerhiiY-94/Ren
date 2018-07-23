#include "test_common.h"

#include "membuf.h"

#include "../Context.h"
#include "../Mesh.h"
#include "../Utils.h"

#ifdef USE_GL_RENDER

#if defined(_WIN32)
#include <Windows.h>
#endif

class MeshTest : public Ren::Context {
#if defined(_WIN32)
    HINSTANCE hInstance;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
#else
    SDL_Window *window_;    void *gl_ctx_;
#endif
public:
    MeshTest() {
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
        wc.lpszClassName = "MeshTest";

        if (!RegisterClass(&wc)) {
            throw std::runtime_error("Cannot register window class!");
        }

        hWnd = CreateWindow("MeshTest", "!!", WS_OVERLAPPEDWINDOW |
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

    ~MeshTest() {
#if defined(_WIN32)
        wglMakeCurrent(NULL, NULL);
        ReleaseDC(hWnd, hDC);
        wglDeleteContext(hRC);
        DestroyWindow(hWnd);
        UnregisterClass("MeshTest", hInstance);
#else
        SDL_GL_DeleteContext(gl_ctx_);
        SDL_DestroyWindow(window_);
#ifndef EMSCRIPTEN
        SDL_Quit();
#endif
#endif
    }
};
#else
#include "../SW/SW.h"
class MeshTest : public Ren::Context {
public:
    MeshTest() {
        Ren::Context::Init(256, 256);
    }
};
#endif

namespace {
unsigned char __ivy_mesh[] = {
    0x53, 0x54, 0x41, 0x54, 0x49, 0x43, 0x5f, 0x4d, 0x45, 0x53, 0x48, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
    0x70, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00,
    0x14, 0x00, 0x00, 0x00, 0x44, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x84, 0x01, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x4d, 0x6f, 0x64, 0x65,
    0x6c, 0x4e, 0x61, 0x6d, 0x65, 0x00, 0x00, 0x00, 0xc3, 0x3c, 0x04, 0x77,
    0x16, 0x50, 0x17, 0x76, 0x00, 0x00, 0x00, 0x00, 0x24, 0x38, 0x28, 0x01,
    0x00, 0x00, 0x28, 0x01, 0xe0, 0x3c, 0x26, 0xc1, 0x99, 0x9b, 0x5c, 0xc3,
    0x32, 0xda, 0xdc, 0xc3, 0x8c, 0xb5, 0xa6, 0x42, 0xc8, 0xd0, 0x33, 0x43,
    0x32, 0xda, 0xdc, 0x43, 0x00, 0x00, 0x00, 0x00, 0xd4, 0x87, 0x22, 0x43,
    0x32, 0xda, 0xdc, 0xc3, 0x82, 0xdb, 0x40, 0xbf, 0x50, 0xf7, 0x27, 0x3f,
    0x69, 0x81, 0x34, 0x3d, 0xd8, 0xe8, 0xcd, 0x38, 0x8d, 0x48, 0x5c, 0x3f,
    0x00, 0x00, 0x00, 0x35, 0xad, 0x62, 0xce, 0x42, 0x32, 0xda, 0xdc, 0x43,
    0x8e, 0x3d, 0x47, 0xbf, 0x41, 0x65, 0x20, 0x3f, 0x50, 0x01, 0x28, 0x3d,
    0x4c, 0xce, 0x7f, 0x3f, 0x4e, 0x87, 0x5f, 0x3f, 0x8c, 0xb5, 0xa6, 0x42,
    0x94, 0xf4, 0xf0, 0x42, 0x32, 0xda, 0xdc, 0x43, 0x9f, 0x71, 0x4f, 0xbe,
    0xf4, 0x1f, 0x7a, 0x3f, 0x0d, 0x61, 0x86, 0x3d, 0x54, 0xf9, 0x7f, 0x3f,
    0x00, 0x00, 0x80, 0x3f, 0xe0, 0x3c, 0x26, 0xc1, 0x99, 0x9b, 0x5c, 0xc3,
    0x32, 0xda, 0xdc, 0xc3, 0x00, 0xe4, 0x7f, 0xbf, 0xce, 0x01, 0xe7, 0x3c,
    0x70, 0x01, 0xb8, 0x3a, 0xf1, 0x9d, 0x06, 0x3c, 0xd8, 0xe8, 0xcd, 0x38,
    0xe0, 0x3c, 0x26, 0xc1, 0x99, 0x9b, 0x5c, 0xc3, 0x32, 0xda, 0xdc, 0x43,
    0x00, 0xde, 0x7f, 0xbf, 0x06, 0x41, 0x03, 0x3d, 0x00, 0x00, 0x00, 0x00,
    0x91, 0xf9, 0x7f, 0x3f, 0xe6, 0xe1, 0x1a, 0x3c, 0x8c, 0xb5, 0xa6, 0x42,
    0xc8, 0xd0, 0x33, 0x43, 0x32, 0xda, 0xdc, 0xc3, 0x9f, 0x71, 0x4f, 0xbe,
    0xf4, 0x1f, 0x7a, 0x3f, 0x0d, 0x61, 0x86, 0x3d, 0x71, 0xea, 0xe3, 0x3b,
    0x00, 0x00, 0x80, 0x3f, 0x04, 0x00, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x73, 0x2f, 0x69, 0x76, 0x79,
    0x5f, 0x30, 0x31, 0x2e, 0x74, 0x67, 0x61, 0x00, 0xff, 0x07, 0x00, 0x00,
    0x00, 0x00, 0x28, 0x01, 0x68, 0xef, 0x83, 0x02, 0x7f, 0x07, 0x00, 0x00,
    0x68, 0xef, 0x83, 0x02, 0x08, 0x2b, 0x28, 0x01, 0x7f, 0x0d, 0x00, 0x00,
    0x00, 0xe4, 0x28, 0x00, 0xc8, 0x8b, 0x04, 0x77, 0x00, 0x00, 0x28, 0x01,
    0xe8, 0x93, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00
};
unsigned int __ivy_mesh_len = 400;

unsigned char __skeletal_mesh[] = {
    0x53, 0x4b, 0x45, 0x4c, 0x45, 0x54, 0x5f, 0x4d, 0x45, 0x53, 0x48, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
    0x78, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00,
    0x16, 0x00, 0x00, 0x00, 0x0e, 0x02, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x4e, 0x02, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x5a, 0x02, 0x00, 0x00,
    0xc8, 0x00, 0x00, 0x00, 0x4d, 0x6f, 0x64, 0x65, 0x6c, 0x4e, 0x61, 0x6d,
    0x65, 0x00, 0xbb, 0xd5, 0xfc, 0x7f, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x9f, 0xc0,
    0x0a, 0xd7, 0x23, 0x3c, 0x00, 0x00, 0xa0, 0x41, 0xeb, 0x51, 0xa0, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x9f, 0x40,
    0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x3e, 0x23, 0x45, 0x94, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x41,
    0xff, 0xff, 0x9f, 0x40, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x3e, 0x00, 0x00, 0x00, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x41, 0xff, 0xff, 0x9f, 0xc0, 0x00, 0x00, 0x80, 0xbf,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x45, 0x94, 0x38,
    0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x41, 0xff, 0xff, 0x9f, 0x40,
    0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0x3e, 0x5d, 0xfb, 0x7f, 0x3f, 0x00, 0x00, 0x80, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x41,
    0xff, 0xff, 0x9f, 0xc0, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x23, 0x45, 0x94, 0x38, 0x5d, 0xfb, 0x7f, 0x3f,
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x9f, 0xc0, 0x00, 0x00, 0x80, 0xbf,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x45, 0x94, 0x38,
    0x94, 0x56, 0x94, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x74, 0x65,
    0x78, 0x74, 0x75, 0x72, 0x65, 0x73, 0x2f, 0x6d, 0x61, 0x74, 0x2e, 0x74,
    0x67, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x42, 0x6f, 0x6e, 0x65, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x04,
    0x35, 0x3f, 0xf4, 0x04, 0x35, 0x3f, 0x42, 0x6f, 0x6e, 0x65, 0x30, 0x32,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xf3, 0x04, 0x35, 0x3f, 0xf4, 0x04, 0x35, 0x3f
};
unsigned int __skeletal_mesh_len = 802;
}

void test_mesh() {

    {
        // Load simple mesh
        membuf sbuf(__ivy_mesh, sizeof(__ivy_mesh));
        std::istream in(&sbuf);

        MeshTest test;

        auto on_program_needed = [&test](const char *name, const char *arg1, const char *arg2) {
            Ren::eProgLoadStatus status;
#if defined(USE_GL_RENDER)
            return test.LoadProgramGLSL(name, nullptr, nullptr, &status);
#elif defined(USE_SW_RENDER)
            Ren::Attribute attrs[] = { {} };
            Ren::Uniform unifs[] = { {} };
            return test.LoadProgramSW(name, nullptr, nullptr, 0, attrs, unifs, &status);
#endif
        };

        auto on_texture_needed = [&test](const char *name) {
            Ren::eTexLoadStatus status;
            Ren::Texture2DParams p;
            return test.LoadTexture2D(name, nullptr, 0, p, &status);
        };

        auto on_material_needed = [&](const char *name) {
            Ren::eMatLoadStatus status;
            return test.LoadMaterial(name, nullptr, &status, on_program_needed, on_texture_needed);
        };

        Ren::MeshRef m_ref = test.LoadMesh("ivy", in, on_material_needed);
        require(m_ref->type() == Ren::MeshSimple);
        require(std::string(m_ref->name()) == "ivy");

        require(m_ref->bbox_min() == Ren::Vec3f(-10.389862f, -220.607803f, -441.704651f));
        require(m_ref->bbox_max() == Ren::Vec3f(83.354584f, 179.815552f, 441.704651f));

        require(m_ref->strip(0).offset != -1);
        require(m_ref->strip(1).offset == -1);

        require(m_ref->attribs() != nullptr);
        require(m_ref->attribs_size() == 192);
        require(m_ref->indices() != nullptr);
        require(m_ref->indices_size() == 20);

        require(m_ref->flags() == Ren::MeshHasAlpha);
        require(m_ref->strip(0).flags == Ren::MeshHasAlpha);

        {
            Ren::MeshRef m_ref2 = test.LoadMesh("ivy", in, on_material_needed);
            require(m_ref2);
        }

        Ren::MaterialRef mat_ref = m_ref->strip(0).mat;
        require(!mat_ref->ready());
    }

    {
        // Load skeletal mesh
        membuf sbuf(__skeletal_mesh, sizeof(__skeletal_mesh));
        std::istream in(&sbuf);

        MeshTest test;

        auto on_program_needed = [&test](const char *name, const char *arg1, const char *arg2) {
            Ren::eProgLoadStatus status;
#if defined(USE_GL_RENDER)
            return test.LoadProgramGLSL(name, nullptr, nullptr, &status);
#elif defined(USE_SW_RENDER)
            Ren::Attribute _attrs[] = { {} };
            Ren::Uniform _unifs[] = { {} };
            return test.LoadProgramSW(name, nullptr, nullptr, 0, _attrs, _unifs, &status);
#endif
        };

        auto on_texture_needed = [&test](const char *name) {
            Ren::eTexLoadStatus status;
            Ren::Texture2DParams p;
            return test.LoadTexture2D(name, nullptr, 0, p, &status);
        };

        auto on_material_needed = [&](const char *name) {
            Ren::eMatLoadStatus status;
            return test.LoadMaterial(name, nullptr, &status, on_program_needed, on_texture_needed);
        };

        Ren::MeshRef m_ref = test.LoadMesh("test", in, on_material_needed);
        require(m_ref);
        require(m_ref->type() == Ren::MeshSkeletal);
        require(std::string(m_ref->name()) == "test");

        require(m_ref->bbox_min()[0] == Approx(0).epsilon(0.01));
        require(m_ref->bbox_min()[1] == Approx(0).epsilon(0.01));
        require(m_ref->bbox_min()[2] == Approx(-5).epsilon(0.01));
        require(m_ref->bbox_max()[0] == Approx(0).epsilon(0.01));
        require(m_ref->bbox_max()[1] == Approx(20).epsilon(0.01));
        require(m_ref->bbox_max()[2] == Approx(5).epsilon(0.01));

        require(m_ref->strip(0).offset != -1);
        require(m_ref->strip(1).offset == -1);

        require(m_ref->attribs() != nullptr);
        // attribs have 16 floats for each vertex (3 pos, 3 normal, 2 uvs, 4 bone index-weight pairs)
        require(m_ref->attribs_size() == 4 * 16 * 6);
        require(m_ref->indices() != nullptr);
        require(m_ref->indices_size() == 22);

        require(m_ref->flags() == 0);
        require(m_ref->strip(0).flags == 0);

        require(m_ref->skel()->bones.size() == 2);
        require(std::string(m_ref->skel()->bones[0].name) == "Bone01");
        require(m_ref->skel()->bones[0].id == 0);
        require(m_ref->skel()->bones[0].parent_id == -1);
        require(m_ref->skel()->bones[0].dirty == 1);

        require(std::string(m_ref->skel()->bones[1].name) == "Bone02");
        require(m_ref->skel()->bones[1].id == 1);
        require(m_ref->skel()->bones[1].parent_id == 0);
        require(m_ref->skel()->bones[1].dirty == 1);

        {
            Ren::MeshRef m_ref2 = test.LoadMesh("test", in, on_material_needed);
            require(m_ref2);
        }

        Ren::MaterialRef mat_ref = m_ref->strip(0).mat;
        require(mat_ref);
        require(!mat_ref->ready());
    }
}
