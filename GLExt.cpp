#include "GL.h"

#ifdef WIN32
#include <Windows.h>
#endif

GLuint (APIENTRY *glCreateProgram)(void);
void (APIENTRY *glDeleteProgram)(GLuint program);
void (APIENTRY *glUseProgram)(GLuint program);
void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
void (APIENTRY *glLinkProgram)(GLuint program);
void (APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
void (APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
GLint (APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
GLint (APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
void (APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void (APIENTRY *glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
void (APIENTRY *glEnableVertexAttribArray)(GLuint index);
void (APIENTRY *glDisableVertexAttribArray)(GLuint index);

GLuint (APIENTRY *glCreateShader)(GLenum shaderType);
void (APIENTRY *glDeleteShader)(GLuint shader);
void (APIENTRY *glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
void (APIENTRY *glCompileShader)(GLuint shader);
void (APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
void (APIENTRY *glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

void (APIENTRY *glActiveTexture)(GLenum texture);
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
GLenum (APIENTRY *glCheckFramebufferStatus)(GLenum target);

void (APIENTRY *glUniform1f)(GLint location, GLfloat v0);
void (APIENTRY *glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
void (APIENTRY *glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);

void (APIENTRY *glUniform1i)(GLint location, GLint v0);
void (APIENTRY *glUniform2i)(GLint location, GLint v0, GLint v1);
void (APIENTRY *glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);

void (APIENTRY *glUniform3fv)(GLint location, GLsizei count, const GLfloat *value);

void (APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

bool ren::InitGLExtentions() {
#ifdef WIN32
    glCreateProgram = (decltype(glCreateProgram)) wglGetProcAddress("glCreateProgram");
    glDeleteProgram = (decltype(glDeleteProgram)) wglGetProcAddress("glDeleteProgram");
    glUseProgram = (decltype(glUseProgram)) wglGetProcAddress("glUseProgram");
    glAttachShader = (decltype(glAttachShader)) wglGetProcAddress("glAttachShader");
    glLinkProgram = (decltype(glLinkProgram)) wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (decltype(glGetProgramiv)) wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (decltype(glGetProgramInfoLog)) wglGetProcAddress("glGetProgramInfoLog");
    glGetAttribLocation = (decltype(glGetAttribLocation)) wglGetProcAddress("glGetAttribLocation");
    glGetUniformLocation = (decltype(glGetUniformLocation)) wglGetProcAddress("glGetUniformLocation");
    glGetActiveAttrib = (decltype(glGetActiveAttrib)) wglGetProcAddress("glGetActiveAttrib");
    glGetActiveUniform = (decltype(glGetActiveUniform)) wglGetProcAddress("glGetActiveUniform");
    glVertexAttribPointer = (decltype(glVertexAttribPointer))wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (decltype(glEnableVertexAttribArray))wglGetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (decltype(glDisableVertexAttribArray))wglGetProcAddress("glDisableVertexAttribArray");

    glCreateShader = (decltype(glCreateShader)) wglGetProcAddress("glCreateShader");
    glDeleteShader = (decltype(glDeleteShader)) wglGetProcAddress("glDeleteShader");
    glShaderSource = (decltype(glShaderSource)) wglGetProcAddress("glShaderSource");
    glCompileShader = (decltype(glCompileShader)) wglGetProcAddress("glCompileShader");
    glGetShaderiv = (decltype(glGetShaderiv)) wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (decltype(glGetShaderInfoLog)) wglGetProcAddress("glGetShaderInfoLog");

    glActiveTexture = (decltype(glActiveTexture)) wglGetProcAddress("glActiveTexture");
    glGenerateMipmap = (decltype(glGenerateMipmap)) wglGetProcAddress("glGenerateMipmap");

    glGenBuffers = (decltype(glGenBuffers)) wglGetProcAddress("glGenBuffers");
    glDeleteBuffers = (decltype(glDeleteBuffers)) wglGetProcAddress("glDeleteBuffers");
    glBindBuffer = (decltype(glBindBuffer)) wglGetProcAddress("glBindBuffer");
    glBufferData = (decltype(glBufferData)) wglGetProcAddress("glBufferData");
    glBufferSubData = (decltype(glBufferSubData))wglGetProcAddress("glBufferSubData");

    glGenFramebuffers = (decltype(glGenFramebuffers))wglGetProcAddress("glGenFramebuffers");
    glDeleteFramebuffers = (decltype(glDeleteFramebuffers))wglGetProcAddress("glDeleteFramebuffers");
    glBindFramebuffer = (decltype(glBindFramebuffer))wglGetProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (decltype(glFramebufferTexture2D))wglGetProcAddress("glFramebufferTexture2D");

    glGenRenderbuffers = (decltype(glGenRenderbuffers))wglGetProcAddress("glGenRenderbuffers");
    glDeleteRenderbuffers = (decltype(glDeleteRenderbuffers))wglGetProcAddress("glDeleteRenderbuffers");
    glBindRenderbuffer = (decltype(glBindRenderbuffer))wglGetProcAddress("glBindRenderbuffer");
    glRenderbufferStorage = (decltype(glRenderbufferStorage))wglGetProcAddress("glRenderbufferStorage");

    glFramebufferRenderbuffer = (decltype(glFramebufferRenderbuffer))wglGetProcAddress("glFramebufferRenderbuffer");
    glCheckFramebufferStatus = (decltype(glCheckFramebufferStatus))wglGetProcAddress("glCheckFramebufferStatus");

    glUniform1f = (decltype(glUniform1f))wglGetProcAddress("glUniform1f");
    glUniform2f = (decltype(glUniform2f))wglGetProcAddress("glUniform2f");
    glUniform3f = (decltype(glUniform3f))wglGetProcAddress("glUniform3f");

    glUniform1i = (decltype(glUniform1i))wglGetProcAddress("glUniform1i");
    glUniform2i = (decltype(glUniform2i))wglGetProcAddress("glUniform2i");
    glUniform3i = (decltype(glUniform3i))wglGetProcAddress("glUniform3i");

    glUniform3fv = (decltype(glUniform3fv))wglGetProcAddress("glUniform3fv");

    glUniformMatrix4fv = (decltype(glUniformMatrix4fv))wglGetProcAddress("glUniformMatrix4fv");

    return true;
#endif
}