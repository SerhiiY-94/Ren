#include "FrameBuf.h"

#include <stdexcept>

#include <ren/RenderState.h>
#include <sys/Log.h>

R::FrameBuf::FrameBuf(int _w, int _h, R::eTex2DFormat format, R::eTexFilter filter,
                      R::eTexRepeat repeat, bool with_depth)
        : format(format), w(_w), h(_h) {
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glGenTextures(1, &col_tex);
    R::BindTexture(0, col_tex);
    glActiveTexture(GL_TEXTURE0);

    if (format == R::RawRGB888) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    } else if (format == R::RawRGBA8888) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    } else if (format == R::RawR32F) {
#if defined(EMSCRIPTEN)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, NULL);
#endif
    } else {
        throw std::invalid_argument("Wrong format!");
    }

    if (filter == R::NoFilter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } else if (filter == R::Bilinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    if (repeat == R::ClampToEdge) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else if (repeat == R::Repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, col_tex, 0);

    R::CheckError("[Renderer]: create framebuffer 2");

    if (with_depth) {
        unsigned _depth_rb;

        glGenRenderbuffers(1, &_depth_rb);
        glBindRenderbuffer(GL_RENDERBUFFER, _depth_rb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_rb);

        auto s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (s != GL_FRAMEBUFFER_COMPLETE) {
            LOGI("Frambuffer error %i", int(s));
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            throw std::runtime_error("Framebuffer error!");
        }

        depth_rb = _depth_rb;

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    R::CheckError("[Renderer]: create framebuffer 3");
    LOGI("Framebuffer created (%ix%i)", w, h);
}

R::FrameBuf::FrameBuf(FrameBuf &&rhs) {
    *this = std::move(rhs);
}

R::FrameBuf &R::FrameBuf::operator=(FrameBuf &&rhs) {
    format = rhs.format;
    w = rhs.w;
    h = rhs.h;
    fb = rhs.fb;
    col_tex = rhs.col_tex;
    depth_rb = std::move(rhs.depth_rb);

    rhs.format = R::Undefined;
    rhs.w = rhs.h = -1;

    return *this;
}

R::FrameBuf::~FrameBuf() {
    if (format != R::Undefined) {
        glDeleteFramebuffers(1, &fb);
        if (depth_rb.initialized()) {
            unsigned val = depth_rb.GetValue();
            glDeleteRenderbuffers(1, &val);
        }
        R::UnBindTexture(col_tex);
        glDeleteTextures(1, &col_tex);
    }
}
