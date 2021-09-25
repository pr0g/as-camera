# as-camera

A lightweight camera library for simple graphics applications.

## Usage

`as-camera` can be used easily with CMake (either by installing it and using `find_package` or with `FetchContent`).

[as-camera-input](https://github.com/pr0g/as-camera-input) is an experimental input/controller library for `as-camera` (though they need not be used together).

It is necessary to provide whether the camera will be used in a left-handed or right-handed coordinate system. To do this override `handedness()` somewhere in your application (see below).

```c++
namespace asc
{
  Handedness handedness()
  {
    return Handedness::Left; // or Handedness::Right
  }
}
```

## Overview

When a camera is default constructed it will be at the origin looking down the z-axis (y-axis up).

```c++
asc::Camera camera; // default camera initialized at the origin
```

The `pitch` and `yaw` data members (stored in radians) control the orientation of the camera. Modifying these with the camera at the origin will produce a free-look behaviour.

To create a free-look camera that can move around just modify the `pivot` data member. The `pivot` data member is manipulated in world space. This means to move the camera relative to its local axes, input vectors must be transformed by the camera rotation. For example, if the vector `(1, 0, 0)` is added to `pivot`, the effect will be to move the camera one unit to the right in world space (regardless of the current `pitch` or `yaw` values). To move in the reference frame of the camera, first transform `(1, 0, 0)` by `camera.rotation()`.

```c++
// move 1 unit along the camera's local x-axis
camera.pivot += camera.rotation() * as::vec3::axis_x();
```

To create an orbit (or pivot) behaviour modify the `offset` data member as well. The `offset` data member is manipulated in the local space of the camera. To create a simple orbit behaviour with the pivot point centred in the view, add an offset in z to the `offset` data member. Modifying `pitch` and `yaw` will now orbit the `pivot` point.

It may be required that `offset` move up or down (or any direction) in world space. To achieve this, transform the input vector (e.g. `(0, 1, 0)`) by the inverse of the camera rotation.

```c++
camera.offset += as::mat_inverse(camera.rotation) * as::vec3::axis_y();
// note, this is equivalent to...
camera.offset += as::mat_transpose(camera.rotation) * as::vec3::axis_y();
// which is faster
```

To get the actual transform of the camera in world space use `camera.transform()`. This is a combination of the `pivot`, `offset` and current `pitch`/`yaw` values. There is a convenience function `camera.translation()` to return the current position of the camera (equivalent to `camera.transform().translation`).

To obtain the camera view (used as the `v` in the Model View Projection (MvP) transform) call `camera.view()`. It is just the inverse of `camera.transform()`.

### Other

`camera.rotation()` is subtly different to `camera.transform().rotation`. `camera.rotation()` includes a reflection transform to account for the handedness of the coordinate system the camera is used in (this is why `handedness()` must be provided). This is so input code can still treat positive z as 'move forward' even when working in a right handed reference frame where z points out of the screen.

`move_pivot_detached` is a convenience function to support moving the `pivot` without the camera translation changing (it internally will adjust `offset` to keep `camera.translation()` from changing).

For an interactive example using the camera directly please see the repo [sdl-bgfx-imgui-as_1d-nonlinear-transformations](https://github.com/pr0g/sdl-bgfx-imgui-as_1d-nonlinear-transformations) and the scene `Simple Camera`.
