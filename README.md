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
