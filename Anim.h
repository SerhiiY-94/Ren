#pragma once

#include <cstdint>
#include <cstring>

#include <vector>

#include <math/math.hpp>

#include "Storage.h"

namespace ren {
    enum eAnimBoneFlags { AnimHasTranslate = 1 };

    struct AnimBone {
        char		name[64];
        char		parent_name[64];
        int			id = -1;
        int			offset = 0;
        uint32_t	flags = 0;
        math::vec3	cur_pos;
        math::quat	cur_rot;

        AnimBone() { name[0] = parent_name[0] = '\0'; }

        static const size_t alignment = math::quat::alignment;
    };

    struct Bone;

    class AnimSequence : public RefCounter {
        char		name_[64];
        int			fps_ = 0;
        int			len_ = 0;
        int			frame_size_ = 0;
        float		frame_dur_ = 0;
        float		anim_dur_ = 0;
        std::vector<float> frames_;
        math::aligned_vector<AnimBone> bones_;
        bool        ready_ = false;

    public:
        AnimSequence() {
            name_[0] = '\0';
        }

        AnimSequence(const char *name, void *data);

        const char *name() const { return name_; }
        int fps() const { return fps_; }
        int len() const { return len_; }
        int frame_size() const { return frame_size_; }
        float frame_dur() const { return frame_dur_; }
        float anim_dur() const { return anim_dur_; }
        size_t num_bones() const { return bones_.size(); }
        bool ready() const { return ready_; }

        const float *frames() const { return &frames_[0]; }
        const AnimBone *bone(int i) { return &bones_[i]; }

        void Init(const char *name, void *data);

        std::vector<AnimBone *> LinkBones(math::aligned_vector<Bone, math::mat4::alignment> &bones);
        void Update(float delta, float *time);
        void InterpolateFrames(int fr_0, int fr_1, float t);
    };

    typedef StorageRef<AnimSequence> AnimSeqRef;
    typedef Storage<AnimSequence> AnimSeqStorage;

    struct AnimLink {
        float					anim_time = 0;
        AnimSeqRef			    anim;
        std::vector<AnimBone *> anim_bones;
    };

    struct Bone {
        char		name[64];
        int			id = -1;
        int			parent_id = -1;
        bool		dirty = false;
        math::mat4	cur_matrix;
        math::mat4	cur_comb_matrix;
        math::mat4	bind_matrix;
        math::mat4	inv_bind_matrix;
        math::vec3	head_pos;

        Bone() { name[0] = '\0'; }

        static const size_t alignment = math::mat4::alignment;
    };

    struct BoneGroup {
        std::vector<int> strip_ids;
        std::vector<int> bone_ids;
    };

    struct Skeleton {
        math::aligned_vector<Bone>          bones;
        std::vector<AnimLink>               anims;
        math::aligned_vector<math::mat4>    matr_palette;
        std::vector<BoneGroup>              bone_groups;

        math::aligned_vector<Bone>::iterator bone(const char *name) {
            auto b = bones.begin();
            for (; b != bones.end(); ++b) {
                if (strcmp(b->name, name) == 0) {
                    break;
                }
            }
            return b;
        }

        int bone_index(const char *name) {
            auto b = bone(name);
            if (b == bones.end()) {
                return -1;
            } else {
                return int(b - bones.begin());
            }
        }

        math::vec3 bone_pos(const char *name);
        math::vec3 bone_pos(int i);

        void bone_matrix(const char *name, math::mat4 &mat);
        void bone_matrix(int i, math::mat4 &mat);

        int AddAnimSequence(const AnimSeqRef &ref);

        void MarkChildren();
        void ApplyAnim(int id);
        void ApplyAnim(int anim_id1, int anim_id2, float t);
        void UpdateAnim(int anim_id, float delta, float *t);
        void UpdateBones();
    };
}