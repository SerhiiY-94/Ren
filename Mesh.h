#pragma once

#include <array>
#include <memory>

#include "Anim.h"
#include "Material.h"

namespace ren {
    enum eMeshFlags { MeshHasAlpha = 1 };

    struct TriStrip {
        int			offset;
        int			num_indices;
        MaterialRef mat;
        uint32_t	flags;
    };

    enum eMeshType { MeshUndefined, MeshSimple, MeshTerrain, MeshSkeletal };

    typedef std::function<MaterialRef(const char *name)> material_load_callback;

    class Mesh : public RefCounter {
        int				type_ = MeshUndefined;
        uint32_t		flags_ = 0;
#if defined(USE_GL_RENDER) || defined(USE_SW_RENDER)
        uint32_t		attribs_buf_id_ = 0;
        uint32_t		indices_buf_id_ = 0;
#endif
        std::shared_ptr<void> attribs_;
        size_t			attribs_size_ = 0;
        std::shared_ptr<void> indices_;
        size_t			indices_size_ = 0;
        std::array<TriStrip, 16>    strips_;
        glm::vec3 		bbox_min_, bbox_max_;
        char 			name_[32];

        Skeleton        skel_;

        // simple static mesh with normals
        void InitMeshSimple(void *data, material_load_callback on_mat_load);
        // simple mesh with tex index per vertex
        void InitMeshTerrain(void *data, material_load_callback on_mat_load);
        // mesh with 4 bone weights per vertex
        void InitMeshSkeletal(void *data, material_load_callback on_mat_load);

        // split skeletal mesh into chunks to fit uniforms limit in shader
        void SplitMesh(int bones_limit);
    public:
        Mesh() {
            name_[0] = '\0';
        }
        Mesh(const char *name, void *data, material_load_callback on_mat_load);
        Mesh(const Mesh &rhs) = delete;
        Mesh(Mesh &&rhs) { *this = std::move(rhs); }
        ~Mesh();

        Mesh &operator=(const Mesh &rhs) = delete;
        Mesh &operator=(Mesh &&rhs);

        int type() const { return type_; }
        uint32_t flags() const { return flags_; }
#if defined(USE_GL_RENDER) || defined(USE_SW_RENDER)
        uint32_t attribs_buf_id() const { return attribs_buf_id_; }
        uint32_t indices_buf_id() const { return indices_buf_id_; }
#endif
        const void *attribs() const { return attribs_.get(); }
        size_t attribs_size() const { return attribs_size_; }
        const void *indices() const { return indices_.get(); }
        size_t indices_size() const { return indices_size_; }
        const TriStrip &strip(int i) const { return strips_[i]; }
        const glm::vec3 &bbox_min() const { return bbox_min_; }
        const glm::vec3 &bbox_max() const { return bbox_max_; }
        const char *name() const { return &name_[0]; }

        Skeleton *skel() { return &skel_; }

        void Init(const char *name, void *data, material_load_callback on_mat_load);

        static int max_gpu_bones;
    };

    typedef StorageRef<Mesh> MeshRef;
    typedef Storage<Mesh> MeshStorage;
}