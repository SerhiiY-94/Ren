#pragma once

#if defined(__ANDROID__) || defined(__native_client__) || defined(EMSCRIPTEN)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
//#include <GL/glew.h>

#if defined(WIN32)
//#include <Windows.h>

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

#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_UNIFORMS 0x8B86

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_CUBE_MAP 0x8513

#define GL_CLAMP_TO_EDGE 0x812F
#define GL_GENERATE_MIPMAP_HINT 0x8192

#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_WRAP_R 0x8072

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

#define GL_STATIC_DRAW 0x88E4

#ifndef APIENTRY
#define WINAPI      __stdcall
#define APIENTRY    WINAPI
#endif

extern "C" {
    #include <GL/GL.h>

    typedef unsigned int GLenum;
    typedef unsigned int GLuint;
    typedef int GLint;
    typedef short GLshort;
    typedef unsigned char GLubyte;
    typedef unsigned short GLushort;

    typedef char GLchar;
    typedef ptrdiff_t GLsizeiptr;
}

extern GLuint (APIENTRY *glCreateProgram)(void);
extern void (APIENTRY *glDeleteProgram)(GLuint program);
extern void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
extern void (APIENTRY *glLinkProgram)(GLuint program);
extern void (APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
extern void (APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
extern GLint (APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
extern GLint (APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
extern void (APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
extern void (APIENTRY *glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);

extern GLuint (APIENTRY *glCreateShader)(GLenum shaderType);
extern void (APIENTRY *glDeleteShader)(GLuint shader);
extern void (APIENTRY *glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
extern void (APIENTRY *glCompileShader)(GLuint shader);
extern void (APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
extern void (APIENTRY *glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

extern void (APIENTRY *glActiveTexture)(GLenum texture);
extern void (APIENTRY *glGenerateMipmap)(GLenum target);

extern void (APIENTRY *glGenBuffers)(GLsizei n, GLuint * buffers);
extern void (APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint * buffers);
extern void (APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
extern void (APIENTRY *glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);


namespace ren {
    bool InitGLExtentions();
}
#endif
