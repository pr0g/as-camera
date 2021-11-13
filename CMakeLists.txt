cmake_minimum_required(VERSION 3.15)
project(as-camera LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    as
    GIT_REPOSITORY https://github.com/pr0g/as.git
    GIT_TAG        b21d71ca8f2305c055dda064fa3c6de0c8caaf83)
FetchContent_Declare(
    cmake-helpers
    GIT_REPOSITORY https://github.com/pr0g/cmake-helpers.git
    GIT_TAG        origin/main)
FetchContent_MakeAvailable(as cmake-helpers)

list(APPEND CMAKE_MODULE_PATH ${cmake-helpers_SOURCE_DIR})

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE as)
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

include(thh-install)
thh_install_header_only_with_dependencies(${PROJECT_NAME} as-camera)
