#pragma once

#include "as/as-math-ops.hpp"

namespace asc
{

struct Camera
{
    as::vec3_t look_at{0.0f}; // position of camera when zero focal_dist
                              // position of look_at whe non-zero focal_dist
    float yaw{0.0f};
    float pitch{0.0f};
    float focal_dist{0.0f}; // zero gives fps free look,
                            // otherwise orbit about look_at

    as::mat4_t view() const;      // camera view to pass to shader
    as::mat4_t transform() const; // camera transform to use in app
};

inline as::mat4_t Camera::view() const
{
    return as::mat::inverse(transform());
}

inline as::mat4_t Camera::transform() const
{
    return as::mat4::from_vec3(look_at)
         * as::mat4::from_mat3(as::mat3::rotation_zxy(pitch, yaw, 0.0f))
         * as::mat4::from_vec3(as::vec3_t::axis_z(focal_dist));
}

} // namespace asc
