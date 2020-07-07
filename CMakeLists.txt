cmake_minimum_required(VERSION 3.15)
project(as-camera LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    as
    GIT_REPOSITORY https://github.com/pr0g/as.git
    GIT_TAG        origin/main)
FetchContent_Declare(
    bec GIT_REPOSITORY https://github.com/pr0g/Bit-Field-Enum-Class.git)
FetchContent_MakeAvailable(as bec)

add_library(${PROJECT_NAME} INTERFACE)

include(GNUInstallDirs)
include(simple-install.cmake)

target_include_directories(
    ${PROJECT_NAME} INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
    ${PROJECT_NAME} INTERFACE as bec)

install_header_only(${PROJECT_NAME})
