#include "Context.h"

#include <algorithm>

ren::MeshRef ren::Context::LoadMesh(const char *name, void *data, material_load_callback on_mat_load) {
	MeshRef ref;
	for (auto it = meshes_.begin(); it != meshes_.end(); ++it) {
		if (strcmp(it->name(), name) == 0) {
            ref = { &meshes_, it.index() };
		}
	}

	if (!ref) {
		ref = meshes_.Add(name, data, on_mat_load);
	}

	return ref;
}

ren::MaterialRef ren::Context::LoadMaterial(const char *name, const char *mat_src, eMatLoadStatus *status, const program_load_callback &on_prog_load,
																										   const texture_load_callback &on_tex_load) {
	MaterialRef ref;
	for (auto it = materials_.begin(); it != materials_.end(); ++it) {
		if (strcmp(it->name(), name) == 0) {
            ref = { &materials_, it.index() };
		}
	}

	if (!ref) {
		ref = materials_.Add(name, mat_src, status, on_prog_load, on_tex_load);
	} else {
		if (ref->ready()) {
			if (status) *status = MatFound;
		} else if (!ref->ready() && mat_src) {
			ref->Init(name, mat_src, status, on_prog_load, on_tex_load);
		}
	}

	return ref;
}

int ren::Context::NumMaterialsNotReady() {
	return (int)std::count_if(materials_.begin(), materials_.end(), [](const Material &m) { return !m.ready(); });
}

void ren::Context::ReleaseMaterials() {
    if (!materials_.Size()) return;
    fprintf(stderr, "---------REMAINING MATERIALS--------\n");
    for (const auto &m : materials_) {
        fprintf(stderr, "%s\n", m.name());
    }
    fprintf(stderr, "-----------------------------------\n");
    materials_.Clear();
}

int ren::Context::NumProgramsNotReady() {
	return (int)std::count_if(programs_.begin(), programs_.end(), [](const Program &p) { return !p.ready(); });
}

void ren::Context::ReleasePrograms() {
    if (!programs_.Size()) return;
    fprintf(stderr, "---------REMAINING PROGRAMS--------\n");
    for (const auto &p : programs_) {
#if defined(USE_GL_RENDER) || defined(USE_SW_RENDER)
        fprintf(stderr, "%s %i\n", p.name(), (int)p.prog_id());
#endif
    }
    fprintf(stderr, "-----------------------------------\n");
    programs_.Clear();
}

ren::Texture2DRef ren::Context::LoadTexture2D(const char *name, const void *data, int size, 
                                              const Texture2DParams &p, eTexLoadStatus *load_status) {
	Texture2DRef ref;
	for (auto it = textures_.begin(); it != textures_.end(); ++it) {
		if (strcmp(it->name(), name) == 0) {
            ref = { &textures_, it.index() };
			break;
		}
	}

	if (!ref) {
		ref = textures_.Add(name, data, size, p, load_status);
	} else {
		if (ref->ready()) {
			if (load_status) *load_status = TexFound;
		} else if (!ref->ready() && data) {
			ref->Init(name, data, size, p, load_status);
		}
	}

	return ref;
}

ren::Texture2DRef ren::Context::LoadTextureCube(const char *name, const void *data[6], const int size[6], 
                                                const Texture2DParams &p, eTexLoadStatus *load_status) {
    Texture2DRef ref;
    for (auto it = textures_.begin(); it != textures_.end(); ++it) {
        if (strcmp(it->name(), name) == 0) {
            ref = { &textures_, it.index() };
            break;
        }
    }

    if (!ref) {
        ref = textures_.Add(name, data, size, p, load_status);
    } else {
        if (ref->ready()) {
            if (load_status) *load_status = TexFound;
        } else if (!ref->ready() && data) {
            ref->Init(name, data, size, p, load_status);
        }
    }

    return ref;
}

int ren::Context::NumTexturesNotReady() {
	return (int)std::count_if(textures_.begin(), textures_.end(), [](const Texture2D &t) { return !t.ready(); });
}

void ren::Context::ReleaseTextures() {
	if (!textures_.Size()) return;
	fprintf(stderr, "---------REMAINING TEXTURES--------\n");
	for (const auto &t : textures_) {
		fprintf(stderr, "%s\n", t.name());
	}
	fprintf(stderr, "-----------------------------------\n");
	textures_.Clear();
}

ren::AnimSeqRef ren::Context::LoadAnimSequence(const char *name, void *data) {
    AnimSeqRef ref;
    for (auto it = anims_.begin(); it != anims_.end(); ++it) {
        if (strcmp(it->name(), name) == 0) {
            ref = { &anims_, it.index() };
            break;
        }
    }

    if (!ref) {
        ref = anims_.Add(name, data);
    } else {
        if (ref->ready()) {
        } else if (!ref->ready() && data) {
            ref->Init(name, data);
        }
    }

    return ref;
}

int ren::Context::NumAnimsNotReady() {
	return (int)std::count_if(anims_.begin(), anims_.end(), [](const AnimSequence &a) { return !a.ready(); });
}

void ren::Context::ReleaseAnims() {
	if (!anims_.Size()) return;
	fprintf(stderr, "---------REMAINING ANIMS--------\n");
	for (const auto &a : anims_) {
		fprintf(stderr, "%s\n", a.name());
	}
	fprintf(stderr, "-----------------------------------\n");
	anims_.Clear();
}