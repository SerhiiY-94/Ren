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

    float angle_, aspect_, near_, far_;
public:
    Camera() {}
    Camera(const Vec3f &center, const Vec3f &target, const Vec3f &up);

    const Mat4f &view_matrix() const { return view_matrix_; }
    const Mat4f &projection_matrix() const { return projection_matrix_; }

    const Vec3f &world_position() const { return world_position_; }

    float angle() const { return angle_; }
    float aspect() const { return aspect_; }
    float near() const { return near_; }
    float far() const { return far_; }

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
    bool IsInFrustum(const Vec3f &bbox_min, const Vec3f &bbox_max) const;

    // returns radius
    float GetBoundingSphere(Vec3f &out_center) const;

    void Move(const Vec3f &v, float delta_time);
    void Rotate(float rx, float ry, float delta_time);
};
}
