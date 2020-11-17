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
  PushOut     = 1 << 9,
  PullIn      = 1 << 10,
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

enum class Mode
{
  None,
  Orbit,
  Look
};

struct CameraControl
{
  as::vec3 look_at;
  as::vec2i pan_delta;
  as::vec2i dolly_delta;
  int32_t wheel_delta;
  float pitch;
  float yaw;
  float dolly;
  MotionType motion;
  Mode mode;
};

struct CameraProperties
{
  float translate_speed;
  float rotate_speed;
  float orbit_speed;
  float pan_speed;
  float look_smoothness;
  float move_smoothness;
  float dolly_speed;
  bool pan_local = true;
  bool pan_invert_x = true;
  bool pan_invert_y = true;
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

  const bool orbit = !as::almost_equal(camera.focal_dist, 0.0f, 0.01f);

  const auto flatten = [&camera, orbit](const auto& vector) {
    return orbit ? as::vec_normalize(as::vec3(vector.x, 0.0f, vector.z))
                 : vector;
  };

  const auto basis_x = as::mat3_basis_x(orientation);
  const auto basis_y = as::mat3_basis_y(orientation);
  const auto basis_z = as::mat3_basis_z(orientation);

  const auto forward = flatten(basis_z);

  const auto pan_y = orbit           ? forward
                   : props.pan_local ? basis_y
                                     : as::vec3::axis_y();

  const auto delta_pan_x =
    float(control.pan_delta.x) * basis_x * props.pan_speed;
  const auto delta_pan_y = float(control.pan_delta.y) * pan_y * props.pan_speed;

  control.look_at += delta_pan_x * (props.pan_invert_x ? -1.0f : 1.0f);
  control.look_at += delta_pan_y * (props.pan_invert_y ? 1.0f : -1.0f);

  if (control.mode == Mode::Orbit) {
    control.dolly = as::min(control.dolly + float(control.wheel_delta) * props.dolly_speed, 0.0f);
    control.dolly = as::min(control.dolly + float(control.dolly_delta.y) * props.pan_speed, 0.0f);
  }

  if (control.mode == Mode::Look) {
    control.look_at += float(control.wheel_delta) * props.dolly_speed * forward * movement(handedness);
  }

  if ((control.motion & MotionType::PushOut) == MotionType::PushOut) {
    control.dolly -= props.translate_speed * dt;
    control.look_at = camera.transform().translation - basis_z * control.dolly;
  }

  if ((control.motion & MotionType::PullIn) == MotionType::PullIn) {
    control.dolly = as::min(control.dolly + props.translate_speed * dt, 0.0f);
    control.look_at = camera.transform().translation - basis_z * control.dolly;
  }

  if ((control.motion & MotionType::ScrollReset) == MotionType::ScrollReset) {
    control.look_at = camera.transform().translation;
    control.dolly = 0.0f;
  }

  if ((control.motion & MotionType::Forward) == MotionType::Forward) {
    control.look_at += forward * movement(handedness);
  }

  if ((control.motion & MotionType::Backward) == MotionType::Backward) {
    control.look_at -= forward * movement(handedness);
  }

  if ((control.motion & MotionType::Left) == MotionType::Left) {
    control.look_at -= basis_x * props.translate_speed * dt;
  }

  if ((control.motion & MotionType::Right) == MotionType::Right) {
    control.look_at += basis_x * props.translate_speed * dt;
  }

  if ((control.motion & MotionType::Up) == MotionType::Up) {
    if (control.dolly == 0.0f || props.orbit_speed == 0.0f) {
      control.look_at += as::vec3::axis_y() * props.translate_speed * dt;
    } else {
      control.pitch += props.orbit_speed * dt;
    }
  }

  if ((control.motion & MotionType::Down) == MotionType::Down) {
    if (control.dolly == 0.0f || props.orbit_speed == 0.0f) {
      control.look_at -= as::vec3::axis_y() * props.translate_speed * dt;
    } else {
      control.pitch -= props.orbit_speed * dt;
    }
  }

  if ((control.motion & MotionType::ScrollIn) == MotionType::ScrollIn) {
    control.dolly = as::min(control.dolly + props.translate_speed * dt, 0.0f);
  }

  if ((control.motion & MotionType::ScrollOut) == MotionType::ScrollOut) {
    control.dolly -= props.translate_speed * dt;
  }

  auto clamp_rotation = [](const float angle) {
    return std::fmod(angle + as::k_tau, as::k_tau);
  };

  // keep yaw in 0 - 360 range
  control.yaw = clamp_rotation(control.yaw);
  camera.yaw = clamp_rotation(camera.yaw);

  // ensure smooth transition when moving across 0 - 360 boundary
  const float yaw_delta = control.yaw - camera.yaw;
  if (std::abs(yaw_delta) >= as::k_pi) {
    control.yaw -= as::k_tau * as::sign(yaw_delta);
  }

  // clamp pitch to be +-90 degrees
  control.pitch = as::clamp(control.pitch, -as::k_pi * 0.5f, as::k_pi * 0.5f);

  // https://www.gamasutra.com/blogs/ScottLembcke/20180404/316046/Improved_Lerp_Smoothing.php
  const float look_rate = exp2(props.look_smoothness);
  const float look_t = exp2(-look_rate * dt);
  camera.pitch = as::mix(control.pitch, camera.pitch, look_t);
  camera.yaw = as::mix(control.yaw, camera.yaw, look_t);
  const float move_rate = exp2(props.move_smoothness);
  const float move_t = exp2(-move_rate * dt);
  camera.focal_dist = as::mix(control.dolly, camera.focal_dist, move_t);
  camera.look_at = as::vec_mix(control.look_at, camera.look_at, move_t);
}

} // namespace asc
