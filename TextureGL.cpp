#include "TextureGL.h"

#include <memory>

#include "SOIL2/SOIL2.h"

#include "GL.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

namespace Ren {
std::unique_ptr<uint8_t[]> ReadTGAFile(const void *data, int &w, int &h, eTexColorFormat &format);
void CheckError(const char *op);
}

Ren::Texture2D::Texture2D(const char *name, const void *data, int size,
                          const Texture2DParams &p, eTexLoadStatus *load_status) {
    Init(name, data, size, p, load_status);
}

Ren::Texture2D::Texture2D(const char *name, const void *data[6], const int size[6],
                          const Texture2DParams &p, eTexLoadStatus *load_status) {
    Init(name, data, size, p, load_status);
}

Ren::Texture2D::~Texture2D() {
    if (params_.format != Undefined) {
        GLuint gl_tex = (GLuint)tex_id_;
        glDeleteTextures(1, &gl_tex);
    }
}

Ren::Texture2D &Ren::Texture2D::operator=(Ren::Texture2D &&rhs) {
    if (this == &rhs) return *this;

    if (params_.format != Undefined) {
        GLuint gl_tex = (GLuint)tex_id_;
        glDeleteTextures(1, &gl_tex);
    }

    RefCounter::operator=(std::move(rhs));

    tex_id_ = rhs.tex_id_;
    rhs.tex_id_ = 0;
    params_ = rhs.params_;
    rhs.params_ = {};
    ready_ = rhs.ready_;
    rhs.ready_ = false;
    cubemap_ready_ = rhs.cubemap_ready_;
    rhs.cubemap_ready_ = 0;
    strcpy(name_, rhs.name_);
    rhs.name_[0] = '\0';
    return *this;
}

void Ren::Texture2D::Init(const char *name, const void *data, int size,
                          const Texture2DParams &p, eTexLoadStatus *load_status) {
    strcpy(name_, name);

    if (!data) {
        unsigned char cyan[3] = { 0, 255, 255 };
        Texture2DParams _p;
        _p.w = _p.h = 1;
        _p.format = RawRGB888;
        _p.filter = NoFilter;
        _p.repeat = Repeat;
        InitFromRAWData(cyan, _p);
        // mark it as not ready
        ready_ = false;
        if (load_status) *load_status = TexCreatedDefault;
    } else {
        if (strstr(name, ".tga_rgbe") != 0 || strstr(name, ".TGA_RGBE") != 0) {
            InitFromTGA_RGBEFile(data, p);
        } else if (strstr(name, ".tga") != 0 || strstr(name, ".TGA") != 0) {
            InitFromTGAFile(data, p);
        } else if (strstr(name, ".dds") != 0 || strstr(name, ".DDS") != 0) {
            InitFromDDSFile(data, size, p);
        } else {
            InitFromRAWData(data, p);
        }
        ready_ = true;
        if (load_status) *load_status = TexCreatedFromData;
    }
}

void Ren::Texture2D::Init(const char *name, const void *data[6], const int size[6],
                          const Texture2DParams &p, eTexLoadStatus *load_status) {
    strcpy(name_, name);

    if (!data) {
        const unsigned char cyan[3] = { 0, 255, 255 };
        const void *data[6] = { cyan, cyan, cyan, cyan, cyan, cyan };
        Texture2DParams _p;
        _p.w = _p.h = 1;
        _p.format = RawRGB888;
        _p.filter = NoFilter;
        _p.repeat = Repeat;
        InitFromRAWData(data, _p);
        // mark it as not ready
        ready_ = false;
        cubemap_ready_ = 0;
        if (load_status) *load_status = TexCreatedDefault;
    } else {
        if (strstr(name, ".tga") != 0 || strstr(name, ".TGA") != 0) {
            InitFromTGAFile(data, p);
        } else {
            InitFromRAWData(data, p);
        }

        ready_ = (cubemap_ready_ & (1 << 0)) == 1;
        for (int i = 1; i < 6; i++) {
            ready_ = ready_ && ((cubemap_ready_ & (1 << i)) == 1);
        }
        if (load_status) *load_status = TexCreatedFromData;
    }
}

void Ren::Texture2D::InitFromRAWData(const void *data, const Texture2DParams &p) {
    GLuint tex_id;

    if (params_.format == Undefined) {
        glGenTextures(1, &tex_id);
        tex_id_ = tex_id;
    } else {
        tex_id = (GLuint)tex_id_;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    params_ = p;

    if (p.format == RawRGBA8888) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p.w, p.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (p.format == RawRGB888) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p.w, p.h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (p.format == RawLUM8) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, p.w, p.h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    } else if (p.format == RawRGB16F) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, p.w, p.h, 0, GL_RGB, GL_HALF_FLOAT, data);
    } else if (p.format == RawRGB32F) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, p.w, p.h, 0, GL_RGB, GL_FLOAT, data);
    }

    float anisotropy = 4.0f;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
    if (anisotropy > 0.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
    }

    if (p.filter == NoFilter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (p.filter == Bilinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (p.filter == Trilinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (p.filter == BilinearNoMipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if (p.repeat == Repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else if (p.repeat == ClampToEdge) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if (p.filter == Trilinear || p.filter == Bilinear) {
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    CheckError("create texture");
}

void Ren::Texture2D::InitFromTGAFile(const void *data, const Texture2DParams &p) {
    int w = 0, h = 0;
    eTexColorFormat format = Undefined;
    auto image_data = ReadTGAFile(data, w, h, format);

    Texture2DParams _p = p;
    _p.w = w;
    _p.h = h;
    _p.format = format;


    InitFromRAWData(image_data.get(), _p);
}

void Ren::Texture2D::InitFromTGA_RGBEFile(const void *data, const Texture2DParams &p) {
    int w = 0, h = 0;
    eTexColorFormat format = Undefined;
    auto image_data = ReadTGAFile(data, w, h, format);

    Texture2DParams _p = p;
    _p.w = w;
    _p.h = h;
    _p.format = RawRGB16F;

    std::unique_ptr<int16_t[]> fp_data(new int16_t[_p.w * _p.h * 3]);

    auto f32_to_f16 = [](float value) -> int16_t {
        int32_t i;
        memcpy(&i, &value, sizeof(float));

        int32_t s = (i >> 16) & 0x00008000;
        int32_t e = ((i >> 23) & 0x000000ff) - (127 - 15);
        int32_t m = i & 0x007fffff;
        if (e <= 0) {
            if (e < -10) {
                return reinterpret_cast<const uint16_t &>(s);
            }

            m = (m | 0x00800000) >> (1 - e);

            if (m & 0x00001000)
                m += 0x00002000;

            s = s | (m >> 13);
            uint16_t ret;
            memcpy(&ret, &s, sizeof(uint16_t));
            return ret;
        } else if (e == 0xff - (127 - 15)) {
            if (m == 0) {
                s = s | 0x7c00;
                uint16_t ret;
                memcpy(&ret, &s, sizeof(uint16_t));
                return ret;
            } else {
                m >>= 13;

                s = s | 0x7c00 | m | (m == 0);
                uint16_t ret;
                memcpy(&ret, &s, sizeof(uint16_t));
                return ret;
            }
        } else {
            if (m & 0x00001000) {
                m += 0x00002000;

                if (m & 0x00800000) {
                    m = 0;     // overflow in significand,
                    e += 1;     // adjust exponent
                }
            }

            if (e > 30) {
                s = s | 0x7c00;
                uint16_t ret;
                memcpy(&ret, &s, sizeof(uint16_t));
                return ret;
            }

            s = s | (e << 10) | (m >> 13);
            uint16_t ret;
            memcpy(&ret, &s, sizeof(uint16_t));
            return ret;
        }
    };

    for (int i = 0; i < w * h; i++) {
        uint8_t r = image_data[4 * i + 0];
        uint8_t g = image_data[4 * i + 1];
        uint8_t b = image_data[4 * i + 2];
        uint8_t a = image_data[4 * i + 3];

        float f = std::exp2(float(a) - 128.0f);
        float k = 1.0f / 255;

        fp_data[3 * i + 0] = f32_to_f16(k * r * f);
        fp_data[3 * i + 1] = f32_to_f16(k * g * f);
        fp_data[3 * i + 2] = f32_to_f16(k * b * f);
    }

    InitFromRAWData(fp_data.get(), _p);
}

void Ren::Texture2D::InitFromDDSFile(const void *data, int size, const Texture2DParams &p) {
    GLuint tex_id;
    if (params_.format == Undefined) {
        glGenTextures(1, &tex_id);
        tex_id_ = tex_id;
    } else {
        tex_id = (GLuint)tex_id_;
    }

    params_ = p;
    params_.format = Compressed;

    int res = SOIL_load_OGL_texture_from_memory((unsigned char *)data, size, SOIL_LOAD_AUTO, tex_id, SOIL_FLAG_DDS_LOAD_DIRECT);
    assert(res == tex_id);

    ChangeFilter(p.filter, p.repeat);
}

void Ren::Texture2D::InitFromRAWData(const void *data[6], const Texture2DParams &p) {
    assert(p.w > 0 && p.h > 0);
    GLuint tex_id;
    if (params_.format == Undefined) {
        glGenTextures(1, &tex_id);
        tex_id_ = tex_id;
    } else {
        tex_id = (GLuint)tex_id_;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

    params_ = p;

    int w = p.w, h = p.h;

    for (int i = 0; i < 6; i++) {
        if (!data[i]) {
            /*if (!(cubemap_ready_ & (1 << i))) {
                continue;
            }
            cubemap_ready_ &= ~(1 << i);*/
            continue;
        } else {
            cubemap_ready_ |= (1 << i);
        }

        if (params_.format == RawRGBA8888) {
            glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
        } else if (params_.format == RawRGB888) {
            glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
        } else if (params_.format == RawLUM8) {
            glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data[i]);
        }
    }

    auto f = params_.filter;
    if (f == NoFilter) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (f == Bilinear) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (cubemap_ready_ == 0x3F) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (f == Trilinear) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (cubemap_ready_ == 0x3F) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (f == BilinearNoMipmap) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if !defined(GL_ES_VERSION_2_0) && !defined(__EMSCRIPTEN__)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif

    if ((f == Trilinear || f == Bilinear) && (cubemap_ready_ == 0x3F)) {
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}

void Ren::Texture2D::InitFromTGAFile(const void *data[6], const Texture2DParams &p) {
    std::unique_ptr<uint8_t[]> image_data[6];
    const void *_image_data[6] = {};
    int w = 0, h = 0;
    eTexColorFormat format = Undefined;
    for (int i = 0; i < 6; i++) {
        if (data[i]) {
            image_data[i] = ReadTGAFile(data[i], w, h, format);
            _image_data[i] = image_data[i].get();
        }
    }

    Texture2DParams _p = p;
    _p.w = w;
    _p.h = h;
    _p.format = format;

    InitFromRAWData(_image_data, _p);
}

void Ren::Texture2D::ChangeFilter(eTexFilter f, eTexRepeat r) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id_);

    if (f == NoFilter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else if (f == Bilinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (f == Trilinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (f == BilinearNoMipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if (r == Repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else if (r == ClampToEdge) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if (params_.format != Compressed && (f == Trilinear || f == Bilinear)) {
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void Ren::Texture2D::ReadTextureData(eTexColorFormat format, void *out_data) const {
#if defined(__ANDROID__)
#else
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id_);

    if (format == RawRGBA8888) {
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, out_data);
    }
#endif
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif