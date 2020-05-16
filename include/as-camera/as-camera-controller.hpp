#pragma once

#include "as-camera.hpp"
#include "bitfield-enum-class/bitfield-enum-class.hpp"

namespace asc
{

enum class MotionType
{
    None     = 0,
    Forward  = 1 << 0,
    Backward = 1 << 1,
    Left     = 1 << 2,
    Right    = 1 << 3,
    Up       = 1 << 4,
    Down     = 1 << 5
};

} // namespace asc

template<>
struct bec::EnableBitMaskOperators<asc::MotionType>
{
    static const bool enable = true;
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
    float delta_pitch;
    float delta_yaw;
    float delta_dolly;
    MotionType motion;
};

struct CameraProperties
{
    float translate_speed;
    float rotate_speed;
};

inline void update_camera(
    Camera& camera, const CameraControl& control, const CameraProperties& props,
    const float dt, const Handedness handedness)
{
    const as::mat3_t orientation = as::mat3::from_mat4(camera.transform());

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
        camera.look_at += as::mat3::col2(orientation) * movement(handedness);
    }

    if ((control.motion & MotionType::Backward) == MotionType::Backward) {
        camera.look_at -= as::mat3::col2(orientation) * movement(handedness);
    }

    if ((control.motion & MotionType::Left) == MotionType::Left) {
        camera.look_at
            -= as::mat3::col0(orientation) * props.translate_speed * dt;
    }

    if ((control.motion & MotionType::Right) == MotionType::Right) {
        camera.look_at
            += as::mat3::col0(orientation) * props.translate_speed * dt;
    }

    if ((control.motion & MotionType::Up) == MotionType::Up) {
        camera.look_at
            += as::mat3::col1(orientation) * props.translate_speed * dt;
    }

    if ((control.motion & MotionType::Down) == MotionType::Down) {
        camera.look_at
            -= as::mat3::col1(orientation) * props.translate_speed * dt;
    }

    camera.pitch += control.delta_pitch * props.rotate_speed;
    camera.yaw += control.delta_yaw * props.rotate_speed;
    camera.focal_dist += control.delta_dolly * props.translate_speed;
}

} // namespace asc
