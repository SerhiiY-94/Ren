#include "test_common.h"

#include "../Context.h"
#include "../Mesh.h"

#ifdef USE_GL_RENDER

#include <SDL2/SDL.h>

#include "../GL.h"

class MeshTest : public ren::Context {
    SDL_Window *window_;
    void *gl_ctx_;
public:
    MeshTest() {
        SDL_Init(SDL_INIT_VIDEO);

        window_ = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        gl_ctx_ = SDL_GL_CreateContext(window_);

        ren::Context::Init(256, 256);
#ifndef EMSCRIPTEN
        GLenum glew_err = glewInit();
        if (glew_err != GLEW_OK) {
            fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(glew_err));
        }
#endif
    }

    ~MeshTest() {
        SDL_GL_DeleteContext(gl_ctx_);
        SDL_DestroyWindow(window_);
#ifndef EMSCRIPTEN
        SDL_Quit();
#endif
    }
};
#else
#include "../SW/SW.h"
class MeshTest : public ren::Context {
    SWcontext *ctx;
public:
    MeshTest() {
        ctx = swCreateContext(1, 1);
        swMakeCurrent(ctx);
        ren::Context::Init(256, 256);
    }

    ~MeshTest() {
        swDeleteContext(ctx);
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

    {   // Load simple mesh
        MeshTest test;

		auto on_program_needed = [&test](const char *name, const char *arg1, const char *arg2) {
			ren::eProgLoadStatus status;
#if defined(USE_GL_RENDER)
			return test.LoadProgramGLSL(name, nullptr, nullptr, &status);
#elif defined(USE_SW_RENDER)
            ren::Attribute attrs[] = {{}};
            ren::Uniform unifs[] = {{}};
      		return test.LoadProgramSW(name, nullptr, nullptr, 0, attrs, unifs, &status);
#endif
		};

		auto on_texture_needed = [&test](const char *name) {
			ren::eTexLoadStatus status;
			ren::Texture2DParams p;
			return test.LoadTexture2D(name, nullptr, 0, p, &status);
		};

		auto on_material_needed = [&](const char *name) {
			ren::eMatLoadStatus status;
			return test.LoadMaterial(name, nullptr, &status, on_program_needed, on_texture_needed);
		};

		ren::MeshRef m_ref = test.LoadMesh("ivy", __ivy_mesh, on_material_needed);
		assert(m_ref->type() == ren::MeshSimple);
		assert(std::string(m_ref->name()) == "ivy");

		assert(m_ref->bbox_min() == glm::vec3(-10.389862, -220.607803, -441.704651));
		assert(m_ref->bbox_max() == glm::vec3(83.354584, 179.815552, 441.704651));

		assert(m_ref->strip(0).offset != -1);
		assert(m_ref->strip(1).offset == -1);

		assert(m_ref->attribs() != nullptr);
		assert(m_ref->attribs_size() == 192);
		assert(m_ref->indices() != nullptr);
		assert(m_ref->indices_size() == 20);

		assert(m_ref->flags() == ren::MeshHasAlpha);
		assert(m_ref->strip(0).flags == ren::MeshHasAlpha);

        {
			ren::MeshRef m_ref2 = test.LoadMesh("ivy", __ivy_mesh, on_material_needed);
            assert(m_ref2);
        }

		ren::MaterialRef mat_ref = m_ref->strip(0).mat;
		assert(!mat_ref->ready());
    }

    {   // Load skeletal mesh
        MeshTest test;

		auto on_program_needed = [&test](const char *name, const char *arg1, const char *arg2) {
			ren::eProgLoadStatus status;
#if defined(USE_GL_RENDER)
			return test.LoadProgramGLSL(name, nullptr, nullptr, &status);
#elif defined(USE_SW_RENDER)
            ren::Attribute _attrs[] = {{}};
            ren::Uniform _unifs[] = {{}};
            return test.LoadProgramSW(name, nullptr, nullptr, 0, _attrs, _unifs, &status);
#endif
		};

		auto on_texture_needed = [&test](const char *name) {
			ren::eTexLoadStatus status;
			ren::Texture2DParams p;
			return test.LoadTexture2D(name, nullptr, 0, p, &status);
		};

		auto on_material_needed = [&](const char *name) {
			ren::eMatLoadStatus status;
			return test.LoadMaterial(name, nullptr, &status, on_program_needed, on_texture_needed);
		};

		ren::MeshRef m_ref = test.LoadMesh("test", __skeletal_mesh, on_material_needed);
		assert(m_ref);
		assert(m_ref->type() == ren::MeshSkeletal);
		assert(std::string(m_ref->name()) == "test");

		assert(m_ref->bbox_min().x == Approx(0).epsilon(0.01));
		assert(m_ref->bbox_min().y == Approx(0).epsilon(0.01));
		assert(m_ref->bbox_min().z == Approx(-5).epsilon(0.01));
		assert(m_ref->bbox_max().x == Approx(0).epsilon(0.01));
		assert(m_ref->bbox_max().y == Approx(20).epsilon(0.01));
		assert(m_ref->bbox_max().z == Approx(5).epsilon(0.01));

		assert(m_ref->strip(0).offset != -1);
		assert(m_ref->strip(1).offset == -1);

		assert(m_ref->attribs() != nullptr);
        // attribs have 16 floats for each vertex (3 pos, 3 normal, 2 uvs, 4 bone index-weight pairs)
		assert(m_ref->attribs_size() == 4 * 16 * 6);
		assert(m_ref->indices() != nullptr);
		assert(m_ref->indices_size() == 22);

		assert(m_ref->flags() == 0);
		assert(m_ref->strip(0).flags == 0);

		assert(m_ref->skel()->bones.size() == 2);
		assert(std::string(m_ref->skel()->bones[0].name) == "Bone01");
		assert(m_ref->skel()->bones[0].id == 0);
		assert(m_ref->skel()->bones[0].parent_id == -1);
		assert(m_ref->skel()->bones[0].dirty == 1);

		assert(std::string(m_ref->skel()->bones[1].name) == "Bone02");
		assert(m_ref->skel()->bones[1].id == 1);
		assert(m_ref->skel()->bones[1].parent_id == 0);
		assert(m_ref->skel()->bones[1].dirty == 1);

        {
			ren::MeshRef m_ref2 = test.LoadMesh("test", __skeletal_mesh, on_material_needed);
            assert(m_ref2);
        }

		ren::MaterialRef mat_ref = m_ref->strip(0).mat;
        assert(mat_ref);
		assert(!mat_ref->ready());
    }
}
