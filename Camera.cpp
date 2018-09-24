#include "Camera.h"

int Ren::Plane::ClassifyPoint(const float point[3]) const {
    const float epsilon = 0.002f;

    float result = Dot(MakeVec3(point), n) + d;
    if (result > epsilon) {
        return Front;
    } else if (result < -epsilon) {
        return Back;
    }
    return OnPlane;
}

Ren::Camera::Camera(const Vec3f &center, const Vec3f &target, const Vec3f &up) {
    SetupView(center, target, up);
}

void Ren::Camera::SetupView(const Vec3f &center, const Vec3f &target, const Vec3f &up) {
    LookAt(view_matrix_, center, target, up);
    
    world_position_[0] = -Dot(view_matrix_[0], view_matrix_[3]);
    world_position_[1] = -Dot(view_matrix_[1], view_matrix_[3]);
    world_position_[2] = -Dot(view_matrix_[2], view_matrix_[3]);
}

void Ren::Camera::Perspective(float angle, float aspect, float nearr, float farr) {
    is_orthographic_ = false;
    angle_ = angle;
    aspect_ = aspect;
    near_ = nearr;
    far_ = farr;
    PerspectiveProjection(projection_matrix_, angle, aspect, nearr, farr);
}

void Ren::Camera::Orthographic(float left, float right, float top, float down, float nearr, float farr) {
    is_orthographic_ = true;
    OrthographicProjection(projection_matrix_, left, right, top, down, nearr, farr);
}

void Ren::Camera::Move(const Vec3f &v, float delta_time) {
    view_matrix_[3][0] -= v[0] * delta_time;
    view_matrix_[3][1] -= v[1] * delta_time;
    view_matrix_[3][2] -= v[2] * delta_time;

    //world_position_[0] = -(view_matrix_[0]*view_matrix_[12] + view_matrix_[1]*view_matrix_[13] + view_matrix_[2]*view_matrix_[14]);
    //world_position_[1] = -(view_matrix_[4]*view_matrix_[12] + view_matrix_[5]*view_matrix_[13] + view_matrix_[6]*view_matrix_[14]);
    //world_position_[2] = -(view_matrix_[8]*view_matrix_[12] + view_matrix_[9]*view_matrix_[13] + view_matrix_[10]*view_matrix_[14]);
}

void Ren::Camera::Rotate(float rx, float ry, float delta_time) {
    Vec3f front;
    front[0] = -view_matrix_[0][2];
    front[1] = -view_matrix_[1][2];
    front[2] = -view_matrix_[2][2];

    Mat4f rot_matrix(1.0f);

    rot_matrix = Ren::Rotate(rot_matrix, rx * delta_time, Vec3f{ view_matrix_[0][0], view_matrix_[1][0], view_matrix_[2][0] });
    rot_matrix = Ren::Rotate(rot_matrix, ry * delta_time, Vec3f{ view_matrix_[0][1], view_matrix_[1][1], view_matrix_[2][1] });

    Vec3f tr_front;

    tr_front[0] = Dot(front, Vec3f{ view_matrix_[0][0], view_matrix_[0][1], view_matrix_[0][2] });
    tr_front[1] = Dot(front, Vec3f{ view_matrix_[1][0], view_matrix_[1][1], view_matrix_[1][2] });
    tr_front[2] = Dot(front, Vec3f{ view_matrix_[2][0], view_matrix_[2][1], view_matrix_[2][2] });

    LookAt(view_matrix_, world_position_, world_position_ + tr_front, Vec3f{ 0.0f, 1.0f, 0.0f });
}

void Ren::Camera::UpdatePlanes() {
    Mat4f combo_matrix = projection_matrix_ * view_matrix_;

    frustum_planes_[LeftPlane].n[0] = combo_matrix[0][3] + combo_matrix[0][0];
    frustum_planes_[LeftPlane].n[1] = combo_matrix[1][3] + combo_matrix[1][0];
    frustum_planes_[LeftPlane].n[2] = combo_matrix[2][3] + combo_matrix[2][0];
    frustum_planes_[LeftPlane].d = combo_matrix[3][3] + combo_matrix[3][0];

    frustum_planes_[RightPlane].n[0] = combo_matrix[0][3] - combo_matrix[0][0];
    frustum_planes_[RightPlane].n[1] = combo_matrix[1][3] - combo_matrix[1][0];
    frustum_planes_[RightPlane].n[2] = combo_matrix[2][3] - combo_matrix[2][0];
    frustum_planes_[RightPlane].d = combo_matrix[3][3] - combo_matrix[3][0];

    frustum_planes_[TopPlane].n[0] = combo_matrix[0][3] - combo_matrix[0][1];
    frustum_planes_[TopPlane].n[1] = combo_matrix[1][3] - combo_matrix[1][1];
    frustum_planes_[TopPlane].n[2] = combo_matrix[2][3] - combo_matrix[2][1];
    frustum_planes_[TopPlane].d = combo_matrix[3][3] - combo_matrix[3][1];

    frustum_planes_[BottomPlane].n[0] = combo_matrix[0][3] + combo_matrix[0][1];
    frustum_planes_[BottomPlane].n[1] = combo_matrix[1][3] + combo_matrix[1][1];
    frustum_planes_[BottomPlane].n[2] = combo_matrix[2][3] + combo_matrix[2][1];
    frustum_planes_[BottomPlane].d = combo_matrix[3][3] + combo_matrix[3][1];

    frustum_planes_[NearPlane].n[0] = combo_matrix[0][3] + combo_matrix[0][2];
    frustum_planes_[NearPlane].n[1] = combo_matrix[1][3] + combo_matrix[1][2];
    frustum_planes_[NearPlane].n[2] = combo_matrix[2][3] + combo_matrix[2][2];
    frustum_planes_[NearPlane].d = combo_matrix[3][3] + combo_matrix[3][2];

    frustum_planes_[FarPlane].n[0] = combo_matrix[0][3] - combo_matrix[0][2];
    frustum_planes_[FarPlane].n[1] = combo_matrix[1][3] - combo_matrix[1][2];
    frustum_planes_[FarPlane].n[2] = combo_matrix[2][3] - combo_matrix[2][2];
    frustum_planes_[FarPlane].d = combo_matrix[3][3] - combo_matrix[3][2];

    for (int plane = LeftPlane; plane <= FarPlane; plane++) {
        float inv_l = 1.0f
                      / std::sqrt(
                          frustum_planes_[plane].n[0] * frustum_planes_[plane].n[0]
                          + frustum_planes_[plane].n[1] * frustum_planes_[plane].n[1]
                          + frustum_planes_[plane].n[2] * frustum_planes_[plane].n[2]);
        frustum_planes_[plane].n[0] *= inv_l;
        frustum_planes_[plane].n[1] *= inv_l;
        frustum_planes_[plane].n[2] *= inv_l;
        frustum_planes_[plane].d *= inv_l;
    }

    world_position_[0] = -Dot(view_matrix_[0], view_matrix_[3]);
    world_position_[1] = -Dot(view_matrix_[1], view_matrix_[3]);
    world_position_[2] = -Dot(view_matrix_[2], view_matrix_[3]);
}

bool Ren::Camera::IsInFrustum(const float bbox[8][3]) const {
    for (int plane = LeftPlane; plane <= FarPlane; plane++) {
        int in_count = 8;

        for (int i = 0; i < 8; i++) {
            switch (frustum_planes_[plane].ClassifyPoint(&bbox[i][0])) {
            case Back:
                in_count--;
                break;
            }
        }
        if (in_count == 0) {
            return false;
        }
    }
    return true;
}

bool Ren::Camera::IsInFrustum(const Vec3f &bbox_min, const Vec3f &bbox_max) const {
    const float bbox_points[8][3] = {
        bbox_min[0], bbox_min[1], bbox_min[2],
        bbox_max[0], bbox_min[1], bbox_min[2],
        bbox_min[0], bbox_min[1], bbox_max[2],
        bbox_max[0], bbox_min[1], bbox_max[2],
        bbox_min[0], bbox_max[1], bbox_min[2],
        bbox_max[0], bbox_max[1], bbox_min[2],
        bbox_min[0], bbox_max[1], bbox_max[2],
        bbox_max[0], bbox_max[1], bbox_max[2]
    };

    return IsInFrustum(bbox_points);
}

float Ren::Camera::GetBoundingSphere(Vec3f &out_center) const {
    float f = far_,
          n = near_;

    Vec3f fwd = Vec3f{ -view_matrix_[0][2], -view_matrix_[1][2], -view_matrix_[2][2] };

    float k = std::sqrt(1 + (1.0f / (aspect_ * aspect_))) * std::tan(0.5f * angle_ * Ren::Pi<float>() / 180.0f);
    float k_sqr = k * k;
    if (k_sqr >= (f - n) / (f + n)) {
        out_center = world_position_ + fwd * f;
        return f * k;
    } else {
        out_center = world_position_ + fwd * 0.5f * (f + n) * (1 + k_sqr);
        return 0.5f * std::sqrt((f - n) * (f - n) + 2.0f * (f * f + n * n) * k_sqr + (f + n) * (f + n) * k_sqr * k_sqr);
    }
}