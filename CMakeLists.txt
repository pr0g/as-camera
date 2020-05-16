cmake_minimum_required(VERSION 3.15)
project(as-camera LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    as GIT_REPOSITORY https://github.com/pr0g/as.git)
FetchContent_MakeAvailable(as)

FetchContent_Declare(
    bec GIT_REPOSITORY https://github.com/pr0g/Bit-Field-Enum-Class.git)
FetchContent_MakeAvailable(bec)

add_library(${PROJECT_NAME} INTERFACE)

include(GNUInstallDirs)

target_include_directories(
    ${PROJECT_NAME} INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-config
)

install(
    EXPORT ${PROJECT_NAME}-config
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
