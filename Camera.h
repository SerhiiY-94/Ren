#pragma once

#include "MMat.h"

namespace Ren {
enum ePointPos { Front, Back, OnPlane };

struct Plane {
    Vec3f   n;
    float   d;

    int ClassifyPoint(const float point[3]) const;
};

enum eCamPlane {
    LeftPlane, RightPlane, TopPlane, BottomPlane, NearPlane, FarPlane
};

class Camera {
protected:
    Mat4f view_matrix_;
    Mat4f projection_matrix_;

    Vec3f world_position_;

    Plane frustum_planes_[6];
    bool is_orthographic_;

public:
    Camera(const Vec3f &center, const Vec3f &target, const Vec3f &up);

    const Mat4f &view_matrix() const { return view_matrix_; }
    const Mat4f &projection_matrix() const { return projection_matrix_; }

    const Vec3f &world_position() const { return world_position_; }

    const Plane* const frustum_planes() const {
        return frustum_planes_;
    }

    const bool is_orthographic() const {
        return is_orthographic_;
    }

    void Perspective(float angle, float aspect, float near, float far);
    void Orthographic(float left, float right, float top, float down, float near, float far);

    void SetupView(const Vec3f &center, const Vec3f &target, const Vec3f &up);

    void UpdatePlanes();
    bool IsInFrustum(const float bbox[8][3]) const;

    void Move(const Vec3f &v, float delta_time);
    void Rotate(float rx, float ry, float delta_time);

};
}
