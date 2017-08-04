#include "Mesh.h"

#include <ctime>

#if defined(USE_GL_RENDER)
#include "GL.h"
#elif defined(USE_SW_RENDER)
#include "SW/SW.h"
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif


int ren::Mesh::max_gpu_bones = 16;

ren::Mesh::Mesh(const char *name, void *data, material_load_callback on_mat_load) {
    Init(name, data, on_mat_load);
}

ren::Mesh::~Mesh() {
#if defined(USE_GL_RENDER)
    if (type_ != MeshUndefined) {
        GLuint buf = (GLuint)attribs_buf_id_;
        glDeleteBuffers(1, &buf);
        buf = (GLuint)indices_buf_id_;
        glDeleteBuffers(1, &buf);
    }
#endif
}

ren::Mesh &ren::Mesh::operator=(Mesh &&rhs) {
    RefCounter::operator=(std::move(rhs));

#if defined(USE_GL_RENDER)
    if (type_ != MeshUndefined) {
        GLuint buf = (GLuint)attribs_buf_id_;
        glDeleteBuffers(1, &buf);
        buf = (GLuint)indices_buf_id_;
        glDeleteBuffers(1, &buf);
    }
#endif

    type_ = rhs.type_;
    rhs.type_ = MeshUndefined;
    flags_ = rhs.flags_;
    rhs.flags_ = 0;
#if defined(USE_GL_RENDER) || defined(USE_SW_RENDER)
    attribs_buf_id_ = rhs.attribs_buf_id_;
    rhs.attribs_buf_id_ = 0;
    indices_buf_id_ = rhs.indices_buf_id_;
    rhs.indices_buf_id_ = 0;
#endif
    attribs_ = std::move(rhs.attribs_);
    attribs_size_ = rhs.attribs_size_;
    rhs.attribs_size_ = 0;
    indices_ = std::move(rhs.indices_);
    indices_size_ = rhs.indices_size_;
    rhs.indices_size_ = 0;

	// this does not work properly in vs2013
    //strips_ = std::move(rhs.strips_);
	
	for (size_t i = 0; i < strips_.size(); i++) {
		strips_[i] = std::move(rhs.strips_[i]);
	}

    bbox_min_ = rhs.bbox_min_;
    rhs.bbox_min_ = {};
    bbox_max_ = rhs.bbox_max_;
    rhs.bbox_max_ = {};
    strcpy(name_, rhs.name_);
    rhs.name_[0] = '\0';
    skel_ = std::move(rhs.skel_);

    return *this;
}

void ren::Mesh::Init(const char *name, void *data, material_load_callback on_mat_load) {
    strcpy(name_, name);

    char mesh_type_str[12];
    memcpy(mesh_type_str, data, 12);

    if (strcmp(mesh_type_str, "STATIC_MESH\0") == 0) {
        InitMeshSimple(data, on_mat_load);
    } else if (strcmp(mesh_type_str, "TERRAI_MESH\0") == 0) {
        InitMeshTerrain(data, on_mat_load);
    } else if (strcmp(mesh_type_str, "SKELET_MESH\0") == 0) {
        InitMeshSkeletal(data, on_mat_load);
    }
}

#define READ_ADVANCE(dest, p, size) memcpy(dest, p, size); p += size;

void ren::Mesh::InitMeshSimple(void *data, material_load_callback on_mat_load) {
    char *p = (char *)data;

    char mesh_type_str[12];
    READ_ADVANCE(mesh_type_str, p, 12);
    assert(strcmp(mesh_type_str, "STATIC_MESH\0") == 0);

    type_ = MeshSimple;

    enum {
        MESH_INFO_CHUNK = 0,
        VTX_ATTR_CHUNK,
        VTX_NDX_CHUNK,
        MATERIALS_CHUNK,
        STRIPS_CHUNK
    };

    struct ChunkPos {
        int offset;
        int length;
    };

    struct Header {
        int num_chunks;
        ChunkPos p[5];
    } file_header;

    READ_ADVANCE(&file_header, p, sizeof(file_header));

    // Skip name, cant remember why i put it there
    p += 32;

    float temp_f[3];
    READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
    bbox_min_ = math::make_vec3(temp_f);
    READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
    bbox_max_ = math::make_vec3(temp_f);

    attribs_size_ = (size_t)file_header.p[VTX_ATTR_CHUNK].length;
    attribs_.reset(new char[attribs_size_], std::default_delete<char[]>());
    READ_ADVANCE(attribs_.get(), p, attribs_size_);

    indices_size_ = (size_t)file_header.p[VTX_NDX_CHUNK].length;
    indices_.reset(new char[indices_size_], std::default_delete<char[]>());
    READ_ADVANCE(indices_.get(), p, indices_size_);

    std::vector<std::array<char, 64>> material_names((size_t)file_header.p[MATERIALS_CHUNK].length / 64);
    for (auto &n : material_names) {
        READ_ADVANCE(&n[0], p, 64);
    }

    flags_ = 0;

    int num_strips = file_header.p[STRIPS_CHUNK].length / 12;
    for (int i = 0; i < num_strips; i++) {
        int index, num_indices, alpha;
        READ_ADVANCE(&index, p, 4);
        READ_ADVANCE(&num_indices, p, 4);
        READ_ADVANCE(&alpha, p, 4);

        strips_[i].offset = (int)(index * sizeof(unsigned short));
        strips_[i].num_indices = (int)num_indices;
        strips_[i].flags = 0;

        if (alpha) {
            strips_[i].flags |= MeshHasAlpha;
            flags_ |= MeshHasAlpha;
        }

        strips_[i].mat = on_mat_load(&material_names[i][0]);
    }

    if (num_strips < (int)strips_.size()) {
        strips_[num_strips].offset = -1;
    }

#if defined(USE_GL_RENDER)
    GLuint gl_buf;
    glGenBuffers(1, &gl_buf);
    glBindBuffer(GL_ARRAY_BUFFER, gl_buf);
    glBufferData(GL_ARRAY_BUFFER, attribs_size_, attribs_.get(), GL_STATIC_DRAW);
    attribs_buf_id_ = (uint32_t)gl_buf;

    glGenBuffers(1, &gl_buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size_, indices_.get(), GL_STATIC_DRAW);
    indices_buf_id_ = gl_buf;
#elif defined(USE_SW_RENDER)
    attribs_buf_id_ = (uint32_t)swCreateBuffer();
    swBindBuffer(SW_ARRAY_BUFFER, attribs_buf_id_);
    swBufferData(SW_ARRAY_BUFFER, (SWuint)attribs_size_, attribs_.get());

    indices_buf_id_ = (uint32_t)swCreateBuffer();
    swBindBuffer(SW_INDEX_BUFFER, indices_buf_id_);
    swBufferData(SW_INDEX_BUFFER, (SWuint)indices_size_, indices_.get());
#endif
}

void ren::Mesh::InitMeshTerrain(void *data, material_load_callback on_mat_load) {
    char *p = (char *)data;

    char mesh_type_str[12];
    READ_ADVANCE(mesh_type_str, p, 12);
    assert(strcmp(mesh_type_str, "TERRAI_MESH\0") == 0);

    type_ = MeshTerrain;

    enum {
        MESH_INFO_CHUNK = 0,
        VTX_ATTR_CHUNK,
        VTX_NDX_CHUNK,
        MATERIALS_CHUNK,
        STRIPS_CHUNK
    };

    struct ChunkPos {
        int offset;
        int length;
    };

    struct Header {
        int num_chunks;
        ChunkPos p[5];
    } file_header;

    READ_ADVANCE(&file_header, p, sizeof(file_header));

    // Skip name, cant remember why i put it there
    p += 32;

    float temp_f[3];
    READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
    bbox_min_ = math::make_vec3(temp_f);
    READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
    bbox_max_ = math::make_vec3(temp_f);

    attribs_size_ = file_header.p[VTX_ATTR_CHUNK].length + file_header.p[VTX_NDX_CHUNK].length * sizeof(float);
    attribs_.reset(new char[attribs_size_], std::default_delete<char[]>());

    float *p_fattrs = (float *)attribs_.get();
    for (int i = 0; i < file_header.p[VTX_NDX_CHUNK].length; i++) {
        READ_ADVANCE(&p_fattrs[i * 9], p, 8 * sizeof(float));
    }

    for (int i = 0; i < file_header.p[VTX_NDX_CHUNK].length; i++) {
        unsigned char c;
        READ_ADVANCE(&c, p, 1);
        p_fattrs[i * 9 + 8] = float(c);
    }

    indices_size_ = (size_t)file_header.p[VTX_NDX_CHUNK].length;
    indices_.reset(new char[indices_size_], std::default_delete<char[]>());
    READ_ADVANCE(indices_.get(), p, indices_size_);

    std::vector<std::array<char, 64>> material_names((size_t)file_header.p[MATERIALS_CHUNK].length / 64);
    for (auto &n : material_names) {
        READ_ADVANCE(&n[0], p, 64);
    }

    flags_ = 0;

    int num_strips = file_header.p[STRIPS_CHUNK].length / 12;
    for (int i = 0; i < num_strips; i++) {
        int index, num_indices, alpha;
        READ_ADVANCE(&index, p, 4);
        READ_ADVANCE(&num_indices, p, 4);
        READ_ADVANCE(&alpha, p, 4);

        strips_[i].offset = (int)index * sizeof(unsigned short);
        strips_[i].num_indices = (int)num_indices;
        strips_[i].flags = 0;

        if (alpha) {
            strips_[i].flags |= MeshHasAlpha;
            flags_ |= MeshHasAlpha;
        }

        strips_[i].mat = on_mat_load(&material_names[i][0]);
    }

    if (num_strips < (int)strips_.size()) {
        strips_[num_strips].offset = -1;
    }

#if defined(USE_GL_RENDER)
    GLuint gl_buf;
    glGenBuffers(1, &gl_buf);
    glBindBuffer(GL_ARRAY_BUFFER, gl_buf);
    glBufferData(GL_ARRAY_BUFFER, attribs_size_, attribs_.get(), GL_STATIC_DRAW);
    attribs_buf_id_ = gl_buf;

    glGenBuffers(1, &gl_buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size_, indices_.get(), GL_STATIC_DRAW);
    indices_buf_id_ = gl_buf;
#elif defined(USE_SW_RENDER)
    attribs_buf_id_ = (uint32_t)swCreateBuffer();
    swBindBuffer(SW_ARRAY_BUFFER, attribs_buf_id_);
    swBufferData(SW_ARRAY_BUFFER, (SWuint)attribs_size_, attribs_.get());

    indices_buf_id_ = (uint32_t)swCreateBuffer();
    swBindBuffer(SW_INDEX_BUFFER, indices_buf_id_);
    swBufferData(SW_INDEX_BUFFER, (SWuint)indices_size_, indices_.get());
#endif
}

void ren::Mesh::InitMeshSkeletal(void *data, material_load_callback on_mat_load) {
    char *p = (char *)data;

    char mesh_type_str[12];
    READ_ADVANCE(mesh_type_str, p, 12);
    assert(strcmp(mesh_type_str, "SKELET_MESH\0") == 0);

    type_ = MeshSkeletal;

    enum {
        MESH_INFO_CHUNK = 0,
        VTX_ATTR_CHUNK,
        VTX_NDX_CHUNK,
        MATERIALS_CHUNK,
        STRIPS_CHUNK,
        BONES_CHUNK
    };

    struct ChunkPos {
        int offset;
        int length;
    };

    struct Header {
        int num_chunks;
        ChunkPos p[6];
    } file_header;

    READ_ADVANCE(&file_header, p, sizeof(file_header));

    // Skip name, cant remember why i put it there
    p += 32;

    float temp_f[3];
    READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
    bbox_min_ = math::make_vec3(temp_f);
    READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
    bbox_max_ = math::make_vec3(temp_f);

    attribs_size_ = (size_t)file_header.p[VTX_ATTR_CHUNK].length;
    attribs_.reset(new char[attribs_size_], std::default_delete<char[]>());
    READ_ADVANCE(attribs_.get(), p, attribs_size_);

    indices_size_ = (size_t)file_header.p[VTX_NDX_CHUNK].length;
    indices_.reset(new char[indices_size_], std::default_delete<char[]>());
    READ_ADVANCE(indices_.get(), p, indices_size_);

    std::vector<std::array<char, 64>> material_names((size_t)file_header.p[MATERIALS_CHUNK].length / 64);
    for (auto &n : material_names) {
        READ_ADVANCE(&n[0], p, 64);
    }

    flags_ = 0;

    int num_strips = file_header.p[STRIPS_CHUNK].length / 12;
    for (int i = 0; i < num_strips; i++) {
        int index, num_indices, alpha;
        READ_ADVANCE(&index, p, 4);
        READ_ADVANCE(&num_indices, p, 4);
        READ_ADVANCE(&alpha, p, 4);

        strips_[i].offset = (int)index * sizeof(unsigned short);
        strips_[i].num_indices = (int)num_indices;
        strips_[i].flags = 0;

        if (alpha) {
            strips_[i].flags |= MeshHasAlpha;
            flags_ |= MeshHasAlpha;
        }

        strips_[i].mat = on_mat_load(&material_names[i][0]);
    }

    if (num_strips < (int)strips_.size()) {
        strips_[num_strips].offset = -1;
    }

    auto &bones = skel_.bones;

    int num_bones = file_header.p[BONES_CHUNK].length / (64 + 8 + 12 + 16);
    bones.resize((size_t)num_bones);
    for (int i = 0; i < num_bones; i++) {
        float temp_f[4];
        math::vec3 temp_v;
        math::quat temp_q;
        READ_ADVANCE(bones[i].name, p, 64);
        const char *cc = bones[i].name;
        READ_ADVANCE(&bones[i].id, p, sizeof(int));
        READ_ADVANCE(&bones[i].parent_id, p, sizeof(int));

        READ_ADVANCE(&temp_f[0], p, sizeof(math::vec3));
        temp_v = math::make_vec3(&temp_f[0]);
        bones[i].bind_matrix = math::translate(bones[i].bind_matrix, temp_v);
        READ_ADVANCE(&temp_f[0], p, sizeof(math::quat));
        temp_q = math::make_quat(&temp_f[0]);
        bones[i].bind_matrix *= math::to_mat4(temp_q);
        bones[i].inv_bind_matrix = math::inverse(bones[i].bind_matrix);

        if (bones[i].parent_id != -1) {
            bones[i].cur_matrix = bones[bones[i].parent_id].inv_bind_matrix * bones[i].bind_matrix;
            bones[i].head_pos = math::vec3(bones[bones[i].parent_id].inv_bind_matrix * bones[i].bind_matrix[3]);
        } else {
            bones[i].cur_matrix = bones[i].bind_matrix;
            bones[i].head_pos = math::vec3(bones[i].bind_matrix[3]);
        }
        bones[i].cur_comb_matrix = bones[i].cur_matrix;
        bones[i].dirty = true;
    }

    assert(max_gpu_bones);
    if (bones.size() <= (size_t)max_gpu_bones) {
        for (size_t s = 0; s < strips_.size(); s++) {
            if (strips_[s].offset == -1) break;
            BoneGroup grp;
            for (size_t i = 0; i < bones.size(); i++) {
                grp.bone_ids.push_back(i);
            }
            grp.strip_ids.push_back(s);
            grp.strip_ids.push_back(strips_[s].offset / 2);
            grp.strip_ids.push_back(strips_[s].num_indices);
            skel_.bone_groups.push_back(grp);
        }
    } else {
        SplitMesh(max_gpu_bones);
    }

    skel_.matr_palette.resize(skel_.bones.size());

#if defined(USE_GL_RENDER)
    GLuint gl_buf;
    glGenBuffers(1, &gl_buf);
    glBindBuffer(GL_ARRAY_BUFFER, gl_buf);
    glBufferData(GL_ARRAY_BUFFER, attribs_size_, attribs_.get(), GL_STATIC_DRAW);
    attribs_buf_id_ = gl_buf;

    glGenBuffers(1, &gl_buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size_, indices_.get(), GL_STATIC_DRAW);
    indices_buf_id_ = gl_buf;
#elif defined(USE_SW_RENDER)
    attribs_buf_id_ = (uint32_t)swCreateBuffer();
    swBindBuffer(SW_ARRAY_BUFFER, attribs_buf_id_);
    swBufferData(SW_ARRAY_BUFFER, (SWuint)attribs_size_, attribs_.get());

    indices_buf_id_ = (uint32_t)swCreateBuffer();
    swBindBuffer(SW_INDEX_BUFFER, indices_buf_id_);
    swBufferData(SW_INDEX_BUFFER, (SWuint)indices_size_, indices_.get());
#endif
}

#undef READ_ADVANCE

#undef max

void ren::Mesh::SplitMesh(int bones_limit) {
    assert(type_ == MeshSkeletal);

    std::vector<int> bone_ids;
    bone_ids.reserve(12);

    float *vtx_attribs = (float *)attribs_.get();
    size_t num_vtx_attribs = attribs_size_ / sizeof(float);
    unsigned short *vtx_indices = (unsigned short *)indices_.get();
    size_t num_vtx_indices = indices_size_ / sizeof(unsigned short);

    auto t1 = clock();

    for (size_t s = 0; s < strips_.size(); s++) {
        if (strips_[s].offset == -1) break;
        for (int i = (int)strips_[s].offset / 2; i < (int)(strips_[s].offset / 2 + strips_[s].num_indices - 2); i += 1) {
            bone_ids.clear();
            if (vtx_indices[i] == vtx_indices[i + 1] || vtx_indices[i + 1] == vtx_indices[i + 2]) {
                continue;
            }
            for (int j = i; j < i + 3; j++) {
                for (int k = 8; k < 12; k += 1) {
                    if (vtx_attribs[vtx_indices[j] * 16 + k + 4] > 0.00001f) {
                        if (std::find(bone_ids.begin(), bone_ids.end(), (int)vtx_attribs[vtx_indices[j] * 16 + k]) ==
                            bone_ids.end()) {
                            bone_ids.push_back((int)vtx_attribs[vtx_indices[j] * 16 + k]);
                        }
                    }
                }
            }
            ren::BoneGroup *best_fit = nullptr;
            int best_k = std::numeric_limits<int>::max();
            for (auto &g : skel_.bone_groups) {
                bool b = true;
                int k = 0;
                for (size_t j = 0; j < bone_ids.size(); j++) {
                    if (std::find(g.bone_ids.begin(), g.bone_ids.end(), bone_ids[j]) == g.bone_ids.end()) {
                        k++;
                        if (g.bone_ids.size() + k > (size_t) bones_limit) {
                            b = false;
                            break;
                        }
                    }
                }
                if (b && k < best_k) {
                    best_k = k;
                    best_fit = &g;
                }
            }

            if (!best_fit) {
                skel_.bone_groups.emplace_back();
                best_fit = &skel_.bone_groups[skel_.bone_groups.size() - 1];
            }
            for (size_t j = 0; j < bone_ids.size(); j++) {
                if (std::find(best_fit->bone_ids.begin(), best_fit->bone_ids.end(), bone_ids[j]) ==
                    best_fit->bone_ids.end()) {
                    best_fit->bone_ids.push_back(bone_ids[j]);
                }
            }
            if (!best_fit->strip_ids.empty() && s == best_fit->strip_ids[best_fit->strip_ids.size() - 3] &&
                best_fit->strip_ids[best_fit->strip_ids.size() - 2] +
                best_fit->strip_ids[best_fit->strip_ids.size() - 1] - 0 == i) {
                best_fit->strip_ids[best_fit->strip_ids.size() - 1]++;
            } else {
                best_fit->strip_ids.push_back(s);
                if ((i - strips_[s].offset / 2) % 2 == 0) {
                    best_fit->strip_ids.push_back(i);
                } else {
                    best_fit->strip_ids.push_back(-i);
                }
                best_fit->strip_ids.push_back(1);
            }
        }
    }

    printf("%li\n", clock() - t1);
    t1 = clock();

    std::vector<unsigned short> new_indices;
    new_indices.reserve((size_t)(num_vtx_indices * 1.2f));
    for (auto &g : skel_.bone_groups) {
        std::sort(g.bone_ids.begin(), g.bone_ids.end());
        int cur_s = g.strip_ids[0];

        for (size_t i = 0; i < g.strip_ids.size(); i += 3) {
            bool sign = g.strip_ids[i + 1] >= 0;
            if (!sign) {
                g.strip_ids[i + 1] = -g.strip_ids[i + 1];
            }
            int beg = g.strip_ids[i + 1];
            int end = g.strip_ids[i + 1] + g.strip_ids[i + 2] + 2;

            if (g.strip_ids[i] == cur_s && i > 0) {
                new_indices.push_back(new_indices.back());
                new_indices.push_back(vtx_indices[beg]);
                g.strip_ids[i + 2 - 3] += 2;
                if ((!sign && (g.strip_ids[i + 2 - 3] % 2 == 0)) || (sign && (g.strip_ids[i + 2 - 3] % 2 != 0))) {
                    g.strip_ids[i + 2 - 3] += 1;
                    new_indices.push_back(vtx_indices[beg]);

                }
                g.strip_ids[i + 2 - 3] += g.strip_ids[i + 2] + 2;
                g.strip_ids.erase(g.strip_ids.begin() + i, g.strip_ids.begin() + i + 3);
                i -= 3;
            } else {
                cur_s = g.strip_ids[i];
                g.strip_ids[i + 2] += 2;
                g.strip_ids[i + 1] = (int)new_indices.size();
                if (!sign) {
                    g.strip_ids[i + 2] += 1;
                    new_indices.push_back(vtx_indices[beg]);
                }
            }
            new_indices.insert(new_indices.end(), &vtx_indices[beg], &vtx_indices[end]);
        }

    }
    new_indices.shrink_to_fit();
    printf("%li\n", clock() - t1);
    t1 = clock();

    clock_t find_time = 0;

    std::vector<bool> done_bools(num_vtx_attribs);
    std::vector<float> new_attribs(vtx_attribs, vtx_attribs + num_vtx_attribs);
    for (auto &g : skel_.bone_groups) {
        std::vector<int> moved_points;
        for (size_t i = 0; i < g.strip_ids.size(); i += 3) {
            for (int j = g.strip_ids[i + 1]; j < g.strip_ids[i + 1] + g.strip_ids[i + 2]; j++) {
                if (done_bools[new_indices[j]]) {
                    if (&g - &skel_.bone_groups[0] != 0) {
                        bool b = false;
                        for (size_t k = 0; k < moved_points.size(); k += 2) {
                            if (new_indices[j] == moved_points[k]) {
                                new_indices[j] = (unsigned short)moved_points[k + 1];
                                b = true;
                                break;
                            }
                        }
                        if (!b) {
                            new_attribs.insert(new_attribs.end(), &vtx_attribs[new_indices[j] * 16],
                                               &vtx_attribs[new_indices[j] * 16] + 16);
                            moved_points.push_back(new_indices[j]);
                            moved_points.push_back((int)new_attribs.size() / 16 - 1);
                            new_indices[j] = (unsigned short)(new_attribs.size() / 16 - 1);
                        } else {
                            continue;
                        }
                    } else {
                        continue;
                    }
                } else {
                    done_bools[new_indices[j]] = true;
                }
                for (int k = 8; k < 12; k += 1) {
                    if (new_attribs[new_indices[j] * 16 + k + 4] > 0.0f) {
                        int bone_ndx = (int)(std::find(g.bone_ids.begin(), g.bone_ids.end(),
                            (int)new_attribs[new_indices[j] * 16 + k]) - g.bone_ids.begin());
                        new_attribs[new_indices[j] * 16 + k] = (float)bone_ndx;
                    }
                }
            }
        }
    }

    printf("---------------------------\n");
    for (auto &g : skel_.bone_groups) {
        printf("%u\n", (unsigned int)g.strip_ids.size() / 3);
    }
    printf("---------------------------\n");

    printf("%li\n", clock() - t1);
    printf("find_time = %li\n", find_time);
    printf("after bone broups2\n");

    indices_size_ = new_indices.size() * sizeof(unsigned short);
    indices_.reset(new char[indices_size_], std::default_delete<char[]>());
    memcpy(indices_.get(), &new_indices[0], indices_size_);

    attribs_size_ = new_attribs.size() * sizeof(float);
    attribs_.reset(new char[attribs_size_], std::default_delete<char[]>());
    memcpy(attribs_.get(), &new_attribs[0], attribs_size_);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif