#pragma once

#include "as/as-math-ops.hpp"

namespace asc
{

using as::operator""_r;

enum class Handedness : int32_t
{
  Left = 0,
  Right = 1
};

// note: This function is not implemented - it must be provided by the
// consuming application.
Handedness handedness();

struct Camera
{
  as::vec3 pivot{0.0_r}; // pivot point to rotate about
  as::vec3 offset{0.0_r}; // offset relative to pivot
  as::real yaw{0.0_r};
  as::real pitch{0.0_r};

  void set_pivot(const as::vec3& p)
  {
    auto delta = as::affine_inv_transform_pos(transform(), p)
               - as::affine_inv_transform_pos(transform(), pivot);
    offset -= delta;
    pivot = p;
  }

  // view camera transform (v in MVP)
  as::affine view() const;
  // world camera transform
  // note: will include handedness - lh +z, rh -z
  as::affine transform() const;
  // world rotation
  // note: in rh coordinate system rotation() will include reflection,
  // z will always point into the screen
  as::mat3 rotation() const;
  // world translation
  as::vec3 translation() const;
};

inline as::real handedness_sign()
{
  const as::real signs[] = {1.0_r, -1.0_r};
  return signs[static_cast<int32_t>(handedness())];
}

inline as::affine Camera::view() const
{
  return as::affine_inverse(transform());
}

inline as::affine Camera::transform() const
{
  const as::real sign = handedness_sign();
  return as::affine_mul(
    as::affine_mul(
      as::affine_from_vec3(offset), as::affine_from_mat3(as::mat3_rotation_zxy(
                                       pitch * sign, yaw * sign, 0.0f))),
    as::affine_from_vec3(pivot));
}

inline as::mat3 Camera::rotation() const
{
  const as::real sign = handedness_sign();
  // clang-format off
  const auto reflection = as::mat3(1.0_r, 0.0_r, 0.0_r,
                                   0.0_r, 1.0_r, 0.0_r,
                                   0.0_r, 0.0_r, 1.0_r * sign);
  // clang-format on
  return as::mat_mul(reflection, transform().rotation);
}

inline as::vec3 Camera::translation() const
{
  return transform().translation;
}

} // namespace asc
