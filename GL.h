#pragma once

#if defined(__ANDROID__) || defined(__native_client__) || defined(EMSCRIPTEN)
//#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
/*#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif*/
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#else
//#include <GL/glew.h>

#if defined(WIN32)
#include <cstddef>

#define DECLSPEC_IMPORT __declspec(dllimport)

#if !defined(_GDI32_)
#define WINGDIAPI DECLSPEC_IMPORT
#else
#define WINGDIAPI
#endif

#endif

#define GL_MAJOR_VERSION 33307
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
//#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_UNIFORMS 0x8B86

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE_CUBE_MAP 0x8513

#define GL_CLAMP_TO_EDGE 0x812F
#define GL_GENERATE_MIPMAP_HINT 0x8192

#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_WRAP_R 0x8072

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_UNIFORM_BUFFER 0x8A11

#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8

#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6

#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20

#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41

#define GL_FRAMEBUFFER_COMPLETE 0x8CD5

#define GL_FRAMEBUFFER_BINDING 0x8CA6

#define GL_COMPRESSED_RED 0x8225
#define GL_COMPRESSED_RG 0x8226
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C

#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815

#define GL_RGB16F 0x881B
#define GL_RGBA16F 0x881A

#define GL_HALF_FLOAT 5131

#define GL_MULTISAMPLE 0x809D

#define GL_TEXTURE_2D_MULTISAMPLE 0x9100

#ifndef APIENTRY
#if defined(WIN32)
#define WINAPI      __stdcall
#define APIENTRY    WINAPI
#endif
#endif

#ifndef APIENTRYP
//#define APIENTRYP APIENTRY *
#endif

//extern "C" {
#if defined(WIN32)
#include <GL/GL.h>
#else
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;

typedef char GLchar;

typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
//}

#if !defined(__APPLE__)
extern GLuint(APIENTRY *glCreateProgram)(void);
extern void (APIENTRY *glDeleteProgram)(GLuint program);
extern void (APIENTRY *glUseProgram)(GLuint program);
extern void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
extern void (APIENTRY *glLinkProgram)(GLuint program);
extern void (APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
extern void (APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
extern GLint(APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
extern GLint(APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
extern void (APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
extern void (APIENTRY *glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
extern GLuint (APIENTRY *glGetUniformBlockIndex)(GLuint program, const GLchar *uniformBlockName);
extern void (APIENTRY *glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
extern void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
extern void (APIENTRY *glEnableVertexAttribArray)(GLuint index);
extern void (APIENTRY *glDisableVertexAttribArray)(GLuint index);

extern GLuint(APIENTRY *glCreateShader)(GLenum shaderType);
extern void (APIENTRY *glDeleteShader)(GLuint shader);
extern void (APIENTRY *glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
extern void (APIENTRY *glCompileShader)(GLuint shader);
extern void (APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
extern void (APIENTRY *glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
#endif

#if !defined(__APPLE__)
#if !defined(__linux__)
extern void (APIENTRY *glActiveTexture)(GLenum texture);
#endif
extern void (APIENTRY *glGenerateMipmap)(GLenum target);

extern void (APIENTRY *glGenBuffers)(GLsizei n, GLuint * buffers);
extern void (APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint * buffers);
extern void (APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
extern void (APIENTRY *glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
extern void (APIENTRY *glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
extern void (APIENTRY *glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
extern void (APIENTRY *glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);

extern void (APIENTRY *glGenFramebuffers)(GLsizei n, GLuint *ids);
extern void (APIENTRY *glDeleteFramebuffers)(GLsizei n, const GLuint * framebuffers);
extern void (APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
extern void (APIENTRY *glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

extern void (APIENTRY *glGenRenderbuffers)(GLsizei n, GLuint * renderbuffers);
extern void (APIENTRY *glDeleteRenderbuffers)(GLsizei n, const GLuint * renderbuffers);
extern void (APIENTRY *glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
extern void (APIENTRY *glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

extern void (APIENTRY *glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
extern GLenum(APIENTRY *glCheckFramebufferStatus)(GLenum target);

extern void (APIENTRY *glDrawBuffers)(GLsizei n, const GLenum *bufs);
extern void (APIENTRY *glBindFragDataLocation)(GLuint program, GLuint colorNumber, const char * name);

extern void (APIENTRY *glGenVertexArrays)(GLsizei n, GLuint *arrays);
extern void (APIENTRY *glBindVertexArray)(GLuint array);
extern void (APIENTRY *glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);

extern void (APIENTRY *glUniform1f)(GLint location, GLfloat v0);
extern void (APIENTRY *glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
extern void (APIENTRY *glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern void (APIENTRY *glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

extern void (APIENTRY *glUniform1i)(GLint location, GLint v0);
extern void (APIENTRY *glUniform2i)(GLint location, GLint v0, GLint v1);
extern void (APIENTRY *glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);

extern void (APIENTRY *glUniform3fv)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform4fv)(GLint location, GLsizei count, const GLfloat *value);

extern void (APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

extern void (APIENTRY *glTexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
extern void (APIENTRY *glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat,
        GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
extern void (APIENTRY *glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat,
        GLsizei width, GLsizei height);

namespace Ren {
bool InitGLExtentions();
}
#endif
#endif
