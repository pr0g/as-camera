#pragma once

#include "as-camera.hpp"
#include "bec/bitfield-enum-class.hpp"

namespace asc
{

enum class MotionType
{
  // clang-format off
  None        = 0,
  Forward     = 1 << 0,
  Backward    = 1 << 1,
  Left        = 1 << 2,
  Right       = 1 << 3,
  Up          = 1 << 4,
  Down        = 1 << 5,
  ScrollIn    = 1 << 6,
  ScrollOut   = 1 << 7,
  ScrollReset = 1 << 8,
  // clang-format on
};

} // namespace asc

template<>
struct bec::EnableBitMaskOperators<asc::MotionType>
{
  static const bool Enable = true;
};

namespace asc
{

enum class Handedness
{
  Left,
  Right
};

struct CameraControl
{
  as::vec3 look_at;
  float pitch;
  float yaw;
  float dolly;
  MotionType motion;
};

struct CameraProperties
{
  float translate_speed;
  float rotate_speed;
  float look_smoothness;
};

inline void updateCamera(
  Camera& camera, CameraControl& control, const CameraProperties& props,
  const float dt, const Handedness handedness)
{
  const as::mat3 orientation = camera.transform().rotation;

  using bec::operator&;

  const auto movement = [props, dt](const Handedness handedness) {
    switch (handedness) {
      case Handedness::Left:
        return props.translate_speed * dt;
      case Handedness::Right:
        return -props.translate_speed * dt;
      default:
        return 0.0f;
    }
  };

  const auto flatten = [&camera](const auto& vector) {
    return std::abs(camera.focal_dist) > 0.0f
        ? as::vec_normalize(as::vec3(vector.x, 0.0f, vector.z))
        : vector;
  };

  const auto basis_z = as::mat3_basis_z(orientation);
  const auto basis_x = as::mat3_basis_x(orientation);

  const auto forward = flatten(basis_z);
  const auto right = flatten(basis_x);

  if ((control.motion & MotionType::Forward) == MotionType::Forward) {
    control.look_at += forward * movement(handedness);
  }

  if ((control.motion & MotionType::Backward) == MotionType::Backward) {
    control.look_at -= forward * movement(handedness);
  }

  if ((control.motion & MotionType::Left) == MotionType::Left) {
    control.look_at -= right * props.translate_speed * dt;
  }

  if ((control.motion & MotionType::Right) == MotionType::Right) {
    control.look_at += right * props.translate_speed * dt;
  }

  if ((control.motion & MotionType::Up) == MotionType::Up) {
    control.look_at += as::vec3::axis_y() * props.translate_speed * dt;
  }

  if ((control.motion & MotionType::Down) == MotionType::Down) {
    control.look_at -= as::vec3::axis_y() * props.translate_speed * dt;
  }

  if ((control.motion & MotionType::ScrollIn) == MotionType::ScrollIn) {
    control.dolly = as::min(control.dolly + props.translate_speed * dt, 0.0f);
  }

  if ((control.motion & MotionType::ScrollOut) == MotionType::ScrollOut) {
    control.dolly -= props.translate_speed * dt;
  }

  if ((control.motion & MotionType::ScrollReset) == MotionType::ScrollReset) {
    control.look_at = camera.transform().translation;
    control.dolly = 0.0f;
  }

  // https://www.gamasutra.com/blogs/ScottLembcke/20180404/316046/Improved_Lerp_Smoothing.php
  const float rate = exp2(props.look_smoothness);
  const float t = exp2(-rate * dt);
  camera.pitch = as::mix(control.pitch, camera.pitch, t);
  camera.yaw = as::mix(control.yaw, camera.yaw, t);
  camera.focal_dist = as::mix(control.dolly, camera.focal_dist, t);
  camera.look_at = as::vec_mix(control.look_at, camera.look_at, t);
}

} // namespace asc
