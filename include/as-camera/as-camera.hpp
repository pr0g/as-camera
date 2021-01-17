#pragma once

#include "as/as-math-ops.hpp"

namespace asc
{

using as::operator""_r;

struct Camera
{
  as::vec3 look_at{0.0_r}; // position of camera when look_dist is zero,
                           // or position of look_at when look_dist is greater
                           // than zero
  as::real yaw{0.0_r};
  as::real pitch{0.0_r};
  as::real look_dist{0.0_r}; // zero gives fps free look,
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
      as::affine(as::vec3::axis_z(look_dist)),
      as::affine(as::mat3_rotation_zxy(pitch, yaw, 0.0_r))),
    as::affine(look_at));
}

} // namespace asc
