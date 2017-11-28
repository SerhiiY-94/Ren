#include "GL.h"

#include <cassert>

#if defined(WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <GL/glx.h>
#endif

GLuint(APIENTRY *glCreateProgram)(void);
void (APIENTRY *glDeleteProgram)(GLuint program);
void (APIENTRY *glUseProgram)(GLuint program);
void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
void (APIENTRY *glLinkProgram)(GLuint program);
void (APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
void (APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
GLint(APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
GLint(APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
void (APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void (APIENTRY *glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
void (APIENTRY *glEnableVertexAttribArray)(GLuint index);
void (APIENTRY *glDisableVertexAttribArray)(GLuint index);

GLuint(APIENTRY *glCreateShader)(GLenum shaderType);
void (APIENTRY *glDeleteShader)(GLuint shader);
void (APIENTRY *glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
void (APIENTRY *glCompileShader)(GLuint shader);
void (APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
void (APIENTRY *glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

#if !defined(__linux__)
void (APIENTRY *glActiveTexture)(GLenum texture);
#endif
void (APIENTRY *glGenerateMipmap)(GLenum target);

void (APIENTRY *glGenBuffers)(GLsizei n, GLuint * buffers);
void (APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint * buffers);
void (APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
void (APIENTRY *glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
void (APIENTRY *glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);

void (APIENTRY *glGenFramebuffers)(GLsizei n, GLuint *ids);
void (APIENTRY *glDeleteFramebuffers)(GLsizei n, const GLuint * framebuffers);
void (APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
void (APIENTRY *glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

void (APIENTRY *glGenRenderbuffers)(GLsizei n, GLuint * renderbuffers);
void (APIENTRY *glDeleteRenderbuffers)(GLsizei n, const GLuint * renderbuffers);
void (APIENTRY *glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
void (APIENTRY *glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

void (APIENTRY *glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLenum(APIENTRY *glCheckFramebufferStatus)(GLenum target);

void (APIENTRY *glUniform1f)(GLint location, GLfloat v0);
void (APIENTRY *glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
void (APIENTRY *glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
void (APIENTRY *glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

void (APIENTRY *glUniform1i)(GLint location, GLint v0);
void (APIENTRY *glUniform2i)(GLint location, GLint v0, GLint v1);
void (APIENTRY *glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);

void (APIENTRY *glUniform3fv)(GLint location, GLsizei count, const GLfloat *value);
void (APIENTRY *glUniform4fv)(GLint location, GLsizei count, const GLfloat *value);

void (APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

bool ren::InitGLExtentions() {

#if defined(WIN32)
#define GetProcAddress(name) (decltype(name))wglGetProcAddress(#name); assert(name)
#elif defined(__linux__)
#define GetProcAddress(name) (decltype(name))glXGetProcAddress((const GLubyte *) #name); assert(name)
#endif

    glCreateProgram = GetProcAddress(glCreateProgram);
    glDeleteProgram = GetProcAddress(glDeleteProgram);
    glUseProgram = GetProcAddress(glUseProgram);
    glAttachShader = GetProcAddress(glAttachShader);
    glLinkProgram = GetProcAddress(glLinkProgram);
    glGetProgramiv = GetProcAddress(glGetProgramiv);
    glGetProgramInfoLog = GetProcAddress(glGetProgramInfoLog);
    glGetAttribLocation = GetProcAddress(glGetAttribLocation);
    glGetUniformLocation = GetProcAddress(glGetUniformLocation);
    glGetActiveAttrib = GetProcAddress(glGetActiveAttrib);
    glGetActiveUniform = GetProcAddress(glGetActiveUniform);
    glVertexAttribPointer = GetProcAddress(glVertexAttribPointer);
    glEnableVertexAttribArray = GetProcAddress(glEnableVertexAttribArray);
    glDisableVertexAttribArray = GetProcAddress(glDisableVertexAttribArray);

    glCreateShader = GetProcAddress(glCreateShader);
    glDeleteShader = GetProcAddress(glDeleteShader);
    glShaderSource = GetProcAddress(glShaderSource);
    glCompileShader = GetProcAddress(glCompileShader);
    glGetShaderiv = GetProcAddress(glGetShaderiv);
    glGetShaderInfoLog = GetProcAddress(glGetShaderInfoLog);

#if !defined(__linux__)
    glActiveTexture = GetProcAddress(glActiveTexture);
#endif
    glGenerateMipmap = GetProcAddress(glGenerateMipmap);

    glGenBuffers = GetProcAddress(glGenBuffers);
    glDeleteBuffers = GetProcAddress(glDeleteBuffers);
    glBindBuffer = GetProcAddress(glBindBuffer);
    glBufferData = GetProcAddress(glBufferData);
    glBufferSubData = GetProcAddress(glBufferSubData);

    glGenFramebuffers = GetProcAddress(glGenFramebuffers);
    glDeleteFramebuffers = GetProcAddress(glDeleteFramebuffers);
    glBindFramebuffer = GetProcAddress(glBindFramebuffer);
    glFramebufferTexture2D = GetProcAddress(glFramebufferTexture2D);

    glGenRenderbuffers = GetProcAddress(glGenRenderbuffers);
    glDeleteRenderbuffers = GetProcAddress(glDeleteRenderbuffers);
    glBindRenderbuffer = GetProcAddress(glBindRenderbuffer);
    glRenderbufferStorage = GetProcAddress(glRenderbufferStorage);

    glFramebufferRenderbuffer = GetProcAddress(glFramebufferRenderbuffer);
    glCheckFramebufferStatus = GetProcAddress(glCheckFramebufferStatus);

    glUniform1f = GetProcAddress(glUniform1f);
    glUniform2f = GetProcAddress(glUniform2f);
    glUniform3f = GetProcAddress(glUniform3f);
    glUniform4f = GetProcAddress(glUniform4f);

    glUniform1i = GetProcAddress(glUniform1i);
    glUniform2i = GetProcAddress(glUniform2i);
    glUniform3i = GetProcAddress(glUniform3i);

    glUniform3fv = GetProcAddress(glUniform3fv);
    glUniform4fv = GetProcAddress(glUniform4fv);

    glUniformMatrix4fv = GetProcAddress(glUniformMatrix4fv);

    return true;
}