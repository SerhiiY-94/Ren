#include "TextureGL.h"

#include <memory>

#include <SOIL2/SOIL2.h>

#include "GL.h"

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4996)
#endif

namespace ren {
	std::unique_ptr<uint8_t[]> ReadTGAFile(const void *data, int &w, int &h, eTex2DFormat &format);
	void CheckError(const char *op);
}

ren::Texture2D::Texture2D(const char *name, const void *data, int size, const Texture2DParams &p, eTexLoadStatus *load_status) {
	Init(name, data, size, p, load_status);
}

ren::Texture2D::~Texture2D() {
    if (params_.format != Undefined) {
        GLuint gl_tex = (GLuint) tex_id_;
        glDeleteTextures(1, &gl_tex);
    }
}

ren::Texture2D &ren::Texture2D::operator=(ren::Texture2D &&rhs) {
    if (this == &rhs) return *this;

    if (params_.format != Undefined) {
        GLuint gl_tex = (GLuint) tex_id_;
        glDeleteTextures(1, &gl_tex);
    }

    RefCounter::operator=(std::move(rhs));

    tex_id_ = rhs.tex_id_;
    rhs.tex_id_ = 0;
    params_ = rhs.params_;
    rhs.params_ = {};
    ready_ = rhs.ready_;
    rhs.ready_ = false;
    strcpy(name_, rhs.name_);
    rhs.name_[0] = '\0';
    return *this;
}

void ren::Texture2D::Init(const char *name, const void *data, int size, const Texture2DParams &p, eTexLoadStatus *load_status) {
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
		if (strstr(name, ".tga") != 0 || strstr(name, ".TGA") != 0) {
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

void ren::Texture2D::InitFromRAWData(const void *data, const Texture2DParams &p) {
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
	}

	float anisotropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	if (anisotropy > 0) {
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

void ren::Texture2D::InitFromTGAFile(const void *data, const Texture2DParams &p) {
	int w = 0, h = 0;
	eTex2DFormat format = Undefined;
	auto image_data = ReadTGAFile(data, w, h, format);

	Texture2DParams _p = p;
	_p.w = w;
	_p.h = h;
	_p.format = format;

	InitFromRAWData(image_data.get(), _p);
}

void ren::Texture2D::InitFromDDSFile(const void *data, int size, const Texture2DParams &p) {
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

void ren::Texture2D::ChangeFilter(eTexFilter f, eTexRepeat r) {
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

#ifdef _MSC_VER
    #pragma warning(pop)
#endif