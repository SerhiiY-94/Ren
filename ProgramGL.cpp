#include "ProgramGL.h"

#include "GL.h"

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4996)
#endif

namespace ren {
	GLuint LoadShader(GLenum shader_type, const char *source);
}

ren::Program::Program(const char *name, const char *vs_source, const char *fs_source, eProgLoadStatus *status) {
	Init(name, vs_source, fs_source, status);
}

ren::Program::~Program() {
    if (prog_id_) {
        GLuint prog = (GLuint) prog_id_;
        glDeleteProgram(prog);
    }
}

ren::Program &ren::Program::operator=(Program &&rhs) {
    if (this == &rhs) return *this;

    if (prog_id_) {
        GLuint prog = (GLuint) prog_id_;
        glDeleteProgram(prog);
    }

    prog_id_ = rhs.prog_id_;
    rhs.prog_id_ = 0;
    attributes_ = std::move(rhs.attributes_);
    uniforms_ = std::move(rhs.uniforms_);
    ready_ = rhs.ready_;
    rhs.ready_ = false;
    strcpy(name_, rhs.name_);
    rhs.name_[0] = '\0';

    return *this;
}

void ren::Program::Init(const char *name, const char *vs_source, const char *fs_source, eProgLoadStatus *status) {
    strcpy(name_, name);
	InitFromGLSL(name, vs_source, fs_source, status);
}

void ren::Program::InitFromGLSL(const char *name, const char *vs_source, const char *fs_source, eProgLoadStatus *status) {
	if (!vs_source || !fs_source) {
		if (status) *status = ProgSetToDefault;
		return;
	}

	assert(!ready_);

	std::string vs_source_str = vs_source,
				fs_source_str = fs_source;

	GLuint v_shader = LoadShader(GL_VERTEX_SHADER, vs_source_str.c_str());
	if (!v_shader) {
		fprintf(stderr, "VertexShader %s error", name);
	}

	GLuint f_shader = LoadShader(GL_FRAGMENT_SHADER, fs_source_str.c_str());
	if (!f_shader) {
		fprintf(stderr, "FragmentShader %s error", name);
	}

	GLuint program = glCreateProgram();
	if (program) {
		glAttachShader(program, v_shader);
		glAttachShader(program, f_shader);
		glLinkProgram(program);
		GLint link_status = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &link_status);
		if (link_status != GL_TRUE) {
			GLint buf_len = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_len);
			if (buf_len) {
				char *buf = (char *)malloc((size_t)buf_len);
				if (buf) {
					glGetProgramInfoLog(program, buf_len, NULL, buf);
					fprintf(stderr, "Could not link program: %s", buf);
					free(buf);
					throw;
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	} else {
		fprintf(stderr, "error");
		throw;
	}

	// Parse attribute and uniform bindings
	struct Binding { std::string name; int loc; };
	std::vector<Binding> attr_bindings, uniform_bindings;
	std::vector<Binding> *cur_bind_target = nullptr;

	const char *delims = " \r\n\t";
	char const* p = vs_source_str.c_str() + vs_source_str.find("/*");
	char const* q = strpbrk(p + 2, delims);
	int pass = 0;

SECOND_PASS:
	for (; p != NULL && q != NULL; q = strpbrk(p, delims)) {
		if (p == q) {
			p = q + 1;
			continue;
		}

		std::string item(p, q);
		if (item == "/*") {
			cur_bind_target = nullptr;
		} else if (item == "*/") {
			break;
		} else if (item == "ATTRIBUTES") {
			cur_bind_target = &attr_bindings;
		} else if (item == "UNIFORMS") {
			cur_bind_target = &uniform_bindings;
		} else if (cur_bind_target) {
			p = q + 1; q = strpbrk(p, delims);
			if (*p != ':') {
				fprintf(stderr, "Error parsing material %s", name);
			}
			p = q + 1; q = strpbrk(p, delims);
			int loc = atoi(p);
			cur_bind_target->push_back({ item, loc });
		}

		if (!q) break;
		p = q + 1;
	}

	if (pass++ == 0) {
		p = fs_source_str.c_str() + fs_source_str.find("/*");
		q = strpbrk(p + 1, delims);
		cur_bind_target = nullptr;
		goto SECOND_PASS;
	}

	for (auto &b : attr_bindings) {
		auto &a = attributes_[b.loc];
		a.loc = glGetAttribLocation(program, b.name.c_str());
		if (a.loc != -1) {
			a.name = b.name;
		}
	}

	for (auto &b : uniform_bindings) {
		auto &u = uniforms_[b.loc];
		u.loc = glGetUniformLocation(program, b.name.c_str());
		if (u.loc != -1) {
			u.name = b.name;
		}
	}

	// Enumerate rest of attributes
	int num;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &num);
	for (int i = 0; i < num; i++) {
		int len;
		GLenum n;
		char name[128];
		glGetActiveAttrib(program, i, 128, &len, &len, &n, name);

		int skip = 0, free_index = -1;
		for (int j = 0; j < MAX_NUM_ATTRIBUTES; j++) {
			if (free_index == -1 && attributes_[j].loc == -1) {
				free_index = j;
			}
			if (attributes_[j].loc != -1 && attributes_[j].name == name) {
				skip = 1;
				break;
			}
		}

		if (!skip && free_index != -1) {
			attributes_[free_index].name = name;
			attributes_[free_index].loc = glGetAttribLocation(program, name);
		}
	}

	printf("PROGRAM %s\n", name);

	// Print all attributes
	printf("\tATTRIBUTES\n");
	for (int i = 0; i < MAX_NUM_ATTRIBUTES; i++) {
		if (attributes_[i].loc == -1) {
			continue;
		}
		printf("\t\t%s : %i\n", attributes_[i].name.c_str(), attributes_[i].loc);
	}

	// Enumerate rest of uniforms
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num);
	for (int i = 0; i < num; i++) {
		int len;
		GLenum n;
		char name[128];
		glGetActiveUniform(program, i, 128, &len, &len, &n, name);

		int skip = 0, free_index = -1;
		for (int j = 0; j < MAX_NUM_UNIFORMS; j++) {
			if (free_index == -1 && uniforms_[j].loc == -1) {
				free_index = j;
			}
			if (uniforms_[j].loc != -1 && uniforms_[j].name == name) {
				skip = 1;
				break;
			}
		}

		if (!skip && free_index != -1) {
			uniforms_[free_index].name = name;
			uniforms_[free_index].loc = glGetUniformLocation(program, name);
		}
	}

	// Print all uniforms
	printf("\tUNIFORMS\n");
	for (int i = 0; i < MAX_NUM_UNIFORMS; i++) {
		if (uniforms_[i].loc == -1) {
			continue;
		}
		printf("\t\t%s : %i\n", uniforms_[i].name.c_str(), uniforms_[i].loc);
	}

	prog_id_ = (uint32_t)program;
    ready_ = true;
	if (status) *status = ProgCreatedFromData;
}

GLuint ren::LoadShader(GLenum shader_type, const char *source) {
	GLuint shader = glCreateShader(shader_type);
	if (shader) {
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char *buf = (char *)malloc((size_t)infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					fprintf(stderr, "Could not compile shader %d: %s", shader_type, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
			throw;
		}
	} else {
		fprintf(stderr, "error");
	}

	GLint info_len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

	if (info_len) {
		char *buf = (char *)malloc((size_t)info_len);
		glGetShaderInfoLog(shader, info_len, NULL, buf);
		fprintf(stderr, "%s", buf);
	}

	return shader;
}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif