#pragma once

#include "as-camera.hpp"
#include "bec/bitfield-enum-class.hpp"

namespace asc
{

enum class MotionType
{
    // clang-format off
    None     = 0,
    Forward  = 1 << 0,
    Backward = 1 << 1,
    Left     = 1 << 2,
    Right    = 1 << 3,
    Up       = 1 << 4,
    Down     = 1 << 5
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
    Camera& camera, const CameraControl& control, const CameraProperties& props,
    const float dt, const Handedness handedness)
{
    const as::mat3_t orientation = camera.transform().rotation;

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

    if ((control.motion & MotionType::Forward) == MotionType::Forward) {
        camera.look_at += as::mat3_basis_z(orientation) * movement(handedness);
    }

    if ((control.motion & MotionType::Backward) == MotionType::Backward) {
        camera.look_at -= as::mat3_basis_z(orientation) * movement(handedness);
    }

    if ((control.motion & MotionType::Left) == MotionType::Left) {
        camera.look_at -=
            as::mat3_basis_x(orientation) * props.translate_speed * dt;
    }

    if ((control.motion & MotionType::Right) == MotionType::Right) {
        camera.look_at +=
            as::mat3_basis_x(orientation) * props.translate_speed * dt;
    }

    if ((control.motion & MotionType::Up) == MotionType::Up) {
        camera.look_at +=
            as::mat3_basis_y(orientation) * props.translate_speed * dt;
    }

    if ((control.motion & MotionType::Down) == MotionType::Down) {
        camera.look_at -=
            as::mat3_basis_y(orientation) * props.translate_speed * dt;
    }

    // https://www.gamasutra.com/blogs/ScottLembcke/20180404/316046/Improved_Lerp_Smoothing.php
    const float rate = exp2(props.look_smoothness);
    const float t = exp2(-rate * dt);
    camera.pitch = as::lerp(t, control.pitch, camera.pitch);
    camera.yaw = as::lerp(t, control.yaw, camera.yaw);
    camera.focal_dist = as::lerp(t, control.dolly, camera.focal_dist);
}

} // namespace asc
