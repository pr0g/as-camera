#pragma once

#include "as/as-math-ops.hpp"

namespace asc
{

using as::operator""_r;

enum class Handedness : int32_t
{
  Left  = 0,
  Right = 1
};

struct Camera
{
  as::vec3 look_at{0.0_r}; // position of camera when look_dist is zero,
                           // or position of look_at when look_dist is greater
                           // than zero
  as::real yaw{0.0_r};
  as::real pitch{0.0_r};
  as::real look_dist{0.0_r}; // zero gives first person free look,
                             // otherwise orbit about look_at

  // view camera transform (v in MVP)
  as::affine view(const Handedness handedness) const;
  // world rotation
  as::mat3 rotation(const Handedness handedness) const;
  // world translation
  as::vec3 translation(const Handedness handedness) const;
  // world camera transform
  as::affine transform(const Handedness handedness) const;
};

inline as::real handedness_sign(const Handedness handedness)
{
  const as::real signs[] = {1.0_r, -1.0_r};
  return signs[static_cast<int32_t>(handedness)];
}

inline as::affine Camera::view(const Handedness handedness) const
{
  return as::affine_inverse(transform(handedness));
}

inline as::affine Camera::transform(const Handedness handedness) const
{
  const as::real sign = handedness_sign(handedness);
  return as::affine_mul(
    as::affine_mul(
      as::affine(as::vec3::axis_z(look_dist * sign)),
      as::affine(as::mat3_rotation_zxy(pitch * sign, yaw * sign, 0.0_r))),
    as::affine(look_at));
}

inline as::mat3 Camera::rotation(const Handedness handedness) const
{
  const as::real sign = handedness_sign(handedness);
  const auto reflection = as::mat3(1.0_r, 0.0_r, 0.0_r,
                                   0.0_r, 1.0_r, 0.0_r,
                                   0.0_r, 0.0_r, 1.0_r * sign);
  return as::mat_mul(reflection, transform(handedness).rotation);
}

inline as::vec3 Camera::translation(const Handedness handedness) const
{
  return transform(handedness).translation;
}

} // namespace asc
