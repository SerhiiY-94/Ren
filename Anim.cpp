#include "Anim.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

ren::AnimSequence::AnimSequence(const char *name, void *data) {
    Init(name, data);
}

void ren::AnimSequence::Init(const char *name, void *data) {
    strcpy(name_, name);
    if (!data) {
        ready_ = false;
        return;
    }

#define READ_ADVANCE(dest, p, size) memcpy(dest, p, size); p += size;

    char *p = (char *)data;

    char str[64];
    READ_ADVANCE(str, p, 12);
    assert(strcmp(str, "ANIM_SEQUEN\0") == 0);

    enum {
        SKELETON_CHUNK,
        ANIM_INFO_CHUNK,
        FRAMES_CHUNK
    };

    struct ChunkPos {
        int offset;
        int length;
    };

    struct Header {
        int num_chunks;
        ChunkPos p[3];
    } file_header;

    READ_ADVANCE(&file_header, p, sizeof(file_header));

    size_t num_bones = (size_t)file_header.p[SKELETON_CHUNK].length / (64 + 64 + 4);
    bones_.resize(num_bones);
    int offset = 0;
    for (size_t i = 0; i < num_bones; i++) {
        bones_[i].id = (int)i;
        bones_[i].flags = 0;
        READ_ADVANCE(bones_[i].name, p, 64);
        READ_ADVANCE(bones_[i].parent_name, p, 64);
        int has_translate_anim = 0;
        READ_ADVANCE(&has_translate_anim, p, 4);
        if (has_translate_anim) bones_[i].flags |= AnimHasTranslate;
        bones_[i].offset = offset;
        if (has_translate_anim) {
            offset += 7;
        } else {
            offset += 4;
        }
    }
    frame_size_ = offset;
    READ_ADVANCE(name_, p, 64);
    READ_ADVANCE(&fps_, p, 4);
    READ_ADVANCE(&len_, p, 4);

    frames_.resize(file_header.p[FRAMES_CHUNK].length / 4);
    READ_ADVANCE(&frames_[0], p, (size_t)file_header.p[FRAMES_CHUNK].length);

    frame_dur_ = 1.0f / fps_;
    anim_dur_ = len_ * frame_dur_;

    ready_ = true;
}

std::vector<ren::AnimBone *> ren::AnimSequence::LinkBones(std::vector<Bone> &_bones) {
    std::vector<AnimBone *> anim_bones;
    anim_bones.reserve(_bones.size());
    for (size_t i = 0; i < _bones.size(); i++) {
        bool added = false;
        for (size_t j = 0; j < this->bones_.size(); j++) {
            if (strcmp(_bones[i].name, this->bones_[j].name) == 0) {
                if (_bones[i].parent_id != -1) {
                    assert(strcmp(_bones[_bones[i].parent_id].name, this->bones_[j].parent_name) == 0);
                }
                anim_bones.push_back(&this->bones_[j]);
                added = true;
                break;
            }
        }
        if (!added) {
            anim_bones.push_back(nullptr);
        }
    }
    return anim_bones;
}

void ren::AnimSequence::Update(float delta, float *time) {
    if (len_ < 2)return;
    *time += delta;

    while (*time > anim_dur_)*time -= anim_dur_;
    while (*time < 0.0f)*time += anim_dur_;

    float frame = *time * (float)fps_;
    int fr_0 = (int)glm::floor(frame);
    int fr_1 = (int)glm::ceil(frame);

    fr_0 = fr_0 % len_;
    fr_1 = fr_1 % len_;
    float t = glm::mod(*time, frame_dur_) / frame_dur_;
    InterpolateFrames(fr_0, fr_1, t);
}

void ren::AnimSequence::InterpolateFrames(int fr_0, int fr_1, float t) {
    for (size_t i = 0; i < bones_.size(); i++) {
        int offset = bones_[i].offset;
        if (bones_[i].flags & AnimHasTranslate) {
            glm::vec3 p1 = glm::make_vec3(&frames_[fr_0 * frame_size_ + offset]);
            glm::vec3 p2 = glm::make_vec3(&frames_[fr_1 * frame_size_ + offset]);
            bones_[i].cur_pos = glm::mix(p1, p2, t);
            offset += 3;
        }
        glm::quat q1 = glm::make_quat(&frames_[fr_0 * frame_size_ + offset]);
        glm::quat q2 = glm::make_quat(&frames_[fr_1 * frame_size_ + offset]);
        bones_[i].cur_rot = glm::mix(q1, q2, t);
    }
}

// skeleton

glm::vec3 ren::Skeleton::bone_pos(const char *name) {
    auto b = bone(name);
    glm::vec3 ret;
    const float *m = &(b->cur_comb_matrix)[0][0];
    /*ret[0] = -(m[0] * m[12] + m[1] * m[13] + m[2] * m[14]);
    ret[1] = -(m[4] * m[12] + m[5] * m[13] + m[6] * m[14]);
    ret[2] = -(m[8] * m[12] + m[9] * m[13] + m[10] * m[14]);*/

    ret[0] = m[12];
    ret[1] = m[13];
    ret[2] = m[14];

    return ret;
}

glm::vec3 ren::Skeleton::bone_pos(int i) {
    auto b = &bones[i];
    glm::vec3 ret;
    const float *m = &(b->cur_comb_matrix)[0][0];
    /*ret[0] = -(m[0] * m[12] + m[1] * m[13] + m[2] * m[14]);
    ret[1] = -(m[4] * m[12] + m[5] * m[13] + m[6] * m[14]);
    ret[2] = -(m[8] * m[12] + m[9] * m[13] + m[10] * m[14]);*/

    ret[0] = m[12];
    ret[1] = m[13];
    ret[2] = m[14];

    return ret;
}

void ren::Skeleton::bone_matrix(const char *name, glm::mat4 &mat) {
    auto b = bone(name);
    UpdateBones();
    assert(b != bones.end());
    mat = b->cur_comb_matrix;
}

void ren::Skeleton::bone_matrix(int i, glm::mat4 &mat) {
    UpdateBones();
    mat = bones[i].cur_comb_matrix;
}

void ren::Skeleton::UpdateBones() {
    for (size_t i = 0; i < bones.size(); i++) {
        if (bones[i].dirty) {
            if (bones[i].parent_id != -1) {
                bones[i].cur_comb_matrix = bones[bones[i].parent_id].cur_comb_matrix * bones[i].cur_matrix;
            } else {
                bones[i].cur_comb_matrix = bones[i].cur_matrix;
            }
            matr_palette[i] = bones[i].cur_comb_matrix * bones[i].inv_bind_matrix;
            bones[i].dirty = false;
        }
    }
}

int ren::Skeleton::AddAnimSequence(const AnimSeqRef &ref) {
    anims.emplace_back();
    auto &a = anims.back();
    a.anim = ref;
    a.anim_time = 0.0f;
    a.anim_bones = anims[anims.size() - 1].anim->LinkBones(bones);
    return int(anims.size() - 1);
}

void ren::Skeleton::MarkChildren() {
    for (size_t i = 0; i < bones.size(); i++) {
        if (bones[i].parent_id != -1 && bones[bones[i].parent_id].dirty) {
            bones[i].dirty = true;
        }
    }
}

void ren::Skeleton::ApplyAnim(int id) {
    for (size_t i = 0; i < bones.size(); i++) {
        if (anims[id].anim_bones[i]) {
            glm::mat4 m(1.0f);
            if (anims[id].anim_bones[i]->flags & AnimHasTranslate) {
                m = glm::translate(m, anims[id].anim_bones[i]->cur_pos);
            } else {
                m = glm::translate(m, bones[i].head_pos);
            }
            m *= glm::toMat4(anims[id].anim_bones[i]->cur_rot);
            bones[i].cur_matrix = m;
            bones[i].dirty = true;
        }
    }
    MarkChildren();
}

void ren::Skeleton::ApplyAnim(int anim_id1, int anim_id2, float t) {
    for (size_t i = 0; i < bones.size(); i++) {
        if (anims[anim_id1].anim_bones[i] || anims[anim_id2].anim_bones[i]) {
            glm::mat4 m(1.0f);
            glm::vec3 pos;
            glm::quat orient;
            if (anims[anim_id1].anim_bones[i]) {
                if (anims[anim_id1].anim_bones[i]->flags & AnimHasTranslate) {
                    pos = anims[anim_id1].anim_bones[i]->cur_pos;
                } else {
                    pos = bones[i].head_pos;
                }
                orient = anims[anim_id1].anim_bones[i]->cur_rot;
            }
            if (anims[anim_id2].anim_bones[i]) {
                if (anims[anim_id2].anim_bones[i]->flags & AnimHasTranslate) {
                    pos = glm::mix(pos, anims[anim_id2].anim_bones[i]->cur_pos, t);
                }
                orient = glm::slerp(orient, anims[anim_id2].anim_bones[i]->cur_rot, t);
            }
            m = glm::translate(m, pos);
            m *= glm::toMat4(orient);
            bones[i].cur_matrix = m;
            bones[i].dirty = true;
        }
    }
    MarkChildren();
}

void ren::Skeleton::UpdateAnim(int anim_id, float delta, float *t) {
    if (!t) {
        t = &anims[anim_id].anim_time;
    }
    anims[anim_id].anim->Update(delta, t);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
