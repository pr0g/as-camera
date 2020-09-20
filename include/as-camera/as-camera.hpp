#pragma once

#include "as/as-math-ops.hpp"

namespace asc
{

struct Camera
{
  as::point3 look_at{0.0f}; // position of camera when zero focal_dist
                            // position of look_at when non-zero focal_dist
  float yaw{0.0f};
  float pitch{0.0f};
  float focal_dist{0.0f}; // zero gives fps free look,
                          // otherwise orbit about look_at

  as::affine view() const; // camera view to pass to shader
  as::affine transform() const; // camera transform to use in app
};

inline as::affine Camera::view() const
{
  return as::affine_inverse(transform());
}

inline as::affine Camera::transform() const
{
  return as::affine_mul(
    as::affine_mul(
      as::affine(as::vec3::axis_z(focal_dist)),
      as::affine(as::mat3_rotation_zxy(pitch, yaw, 0.0f))),
    as::affine_from_point3(look_at));
}

} // namespace asc
