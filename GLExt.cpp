#include "GL.h"

GLuint (APIENTRY *glCreateProgram)(void);
void (APIENTRY *glDeleteProgram)(GLuint program);
void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
void (APIENTRY *glLinkProgram)(GLuint program);
void (APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
void (APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
GLint (APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
GLint (APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
void (APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
void (APIENTRY *glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);

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

bool ren::InitGLExtentions() {
#ifdef WIN32
    glCreateProgram = (decltype(glCreateProgram)) wglGetProcAddress("glCreateProgram");
    glDeleteProgram = (decltype(glDeleteProgram)) wglGetProcAddress("glDeleteProgram");
    glAttachShader = (decltype(glAttachShader)) wglGetProcAddress("glAttachShader");
    glLinkProgram = (decltype(glLinkProgram)) wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (decltype(glGetProgramiv)) wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (decltype(glGetProgramInfoLog)) wglGetProcAddress("glGetProgramInfoLog");
    glGetAttribLocation = (decltype(glGetAttribLocation)) wglGetProcAddress("glGetAttribLocation");
    glGetUniformLocation = (decltype(glGetUniformLocation)) wglGetProcAddress("glGetUniformLocation");
    glGetActiveAttrib = (decltype(glGetActiveAttrib)) wglGetProcAddress("glGetActiveAttrib");
    glGetActiveUniform = (decltype(glGetActiveUniform)) wglGetProcAddress("glGetActiveUniform");

    glCreateShader = (decltype(glCreateShader)) wglGetProcAddress("glCreateShader");
    glDeleteShader = (decltype(glDeleteShader)) wglGetProcAddress("glDeleteShader");
    glShaderSource = (decltype(glShaderSource)) wglGetProcAddress("glShaderSource");
    glCompileShader = (decltype(glCompileShader)) wglGetProcAddress("glCompileShader");
    glGetShaderiv = (decltype(glGetShaderiv)) wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (decltype(glGetShaderInfoLog)) wglGetProcAddress("glGetShaderInfoLog");

    glActiveTexture = (decltype(glActiveTexture)) wglGetProcAddress("glActiveTexture");
    glGenerateMipmap = (decltype(glGenerateMipmap)) wglGetProcAddress("glActiveTexture");

    glGenBuffers = (decltype(glGenBuffers)) wglGetProcAddress("glGenBuffers");
    glDeleteBuffers = (decltype(glDeleteBuffers)) wglGetProcAddress("glDeleteBuffers");
    glBindBuffer = (decltype(glBindBuffer)) wglGetProcAddress("glBindBuffer");
    glBufferData = (decltype(glBufferData)) wglGetProcAddress("glBufferData");

    return true;
#endif
}