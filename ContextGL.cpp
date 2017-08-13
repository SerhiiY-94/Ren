#include "Context.h"

#include "GL.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

ren::Context::~Context() {
    ReleaseAll();
}

void ren::Context::Init(int w, int h) {
    InitGLExtentions();

    w_ = w;
    h_ = h;

    printf("===========================================\n");
    printf("Device info:\n");
    
    // print device info
#if !defined(EMSCRIPTEN) && !defined(__ANDROID__)
    GLint gl_version;
    glGetIntegerv(GL_MAJOR_VERSION, &gl_version);
    printf("\tOpenGL version\t: %i\n", int(gl_version));
#endif
    
    printf("\tVendor\t\t: %s\n", glGetString(GL_VENDOR));
    printf("\tRenderer\t: %s\n", glGetString(GL_RENDERER));
    printf("\tGLSL version\t: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    printf("Capabilities:\n");
    
    // determine if anisotropy supported
    if (IsExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
        GLfloat f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f);
        anisotropy = f;
        printf("\tAnisotropy\t: %f\n", anisotropy);
    }
    
    // how many uniform vec4 vectors can be used
    GLint i = 0;
    glGetIntegerv(/*GL_MAX_VERTEX_UNIFORM_VECTORS*/ GL_MAX_VERTEX_UNIFORM_COMPONENTS, &i);
    i /= 4;
    if (i == 0) i = 256;
    max_uniform_vec4 = i;
    printf("\tMax uniforms\t: %i\n", max_uniform_vec4);
    
    // how many bones(mat4) can be used at time
    Mesh::max_gpu_bones = max_uniform_vec4 / 8;
    printf("\tBones per pass\t: %i\n", Mesh::max_gpu_bones);
    char buff[16];
    sprintf(buff, "%i", Mesh::max_gpu_bones);
    glsl_defines_ += "#define MAX_GPU_BONES ";
    glsl_defines_ += buff;
    glsl_defines_ += "\r\n";
    
    printf("===========================================\n\n");
}

void ren::Context::Resize(int w, int h) {
    w_ = w; h_ = h;
    glViewport(0, 0, w_, h_);
}

ren::ProgramRef ren::Context::LoadProgramGLSL(const char *name, const char *vs_source, const char *fs_source, eProgLoadStatus *load_status) {
    ProgramRef ref;
    for (auto it = programs_.begin(); it != programs_.end(); ++it) {
        if (strcmp(it->name(), name) == 0) {
            ref = { &programs_, it.index() };
            break;
        }
    }

    std::string vs_source_str = glsl_defines_ + (vs_source ? vs_source : ""),
                fs_source_str = glsl_defines_ + (fs_source ? fs_source : "");

    if (vs_source) {
        vs_source_str = glsl_defines_ + vs_source;
        vs_source = vs_source_str.c_str();
    }

    if (fs_source) {
        fs_source_str = glsl_defines_ + fs_source;
        fs_source = fs_source_str.c_str();
    }

    if (!ref) {
        ref = programs_.Add(name, vs_source, fs_source, load_status);
    } else {
        if (ref->ready()) {
            if (load_status) *load_status = ProgFound;
        } else if (!ref->ready() && vs_source && fs_source) {
            ref->Init(name, vs_source, fs_source, load_status);
        }
    }

    return ref;
}

bool ren::Context::IsExtensionSupported(const char *ext) {
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;

    where = (GLubyte *)strchr(ext, ' ');
    if (where || *ext == '\0')
        return 0;
    extensions = glGetString(GL_EXTENSIONS);
    if (!extensions) return 0;
    assert(ext);

    start = extensions;
    for (;;) {
        where = (GLubyte *)strstr((const char *)start, ext);
        if (!where)
            break;
        terminator = where + strlen(ext);
        if (where == start || *(where - 1) == ' ') if (*terminator == ' ' || *terminator == '\0')
            return 1;
        start = terminator;
    }
    return 0;
}

void ren::CheckError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        fprintf(stderr, "after %s glError (0x%x)\n", op, error);
    }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif