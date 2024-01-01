cmake_minimum_required(VERSION 3.11)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

project(kame-proj LANGUAGES C CXX)

option(KAME_BINDING_PYTHON "Build python binding" OFF)

add_subdirectory(thirdparty)

add_library(kame-cpp
    src/kame.cpp
    src/math/vector3.cpp
    src/math/vector4.cpp
    src/math/matrix.cpp
    src/math/quaternion.cpp
    src/sdl/sdl.cpp
    src/sdl/sdl_ogl.cpp
    src/sdl/sdl_vk.cpp
    src/ogl/glad.cpp
    src/ogl/ogl.cpp
    src/vk/vk.cpp
    src/vk/volk.cpp
    src/gltf/gltf.cpp
    src/gltf/gltf_material.cpp
    src/gltf/gltf_ext.cpp
    src/squirtle/squirtle.cpp
    src/squirtle/model.cpp
    src/squirtle/instance.cpp
    src/squirtle/animation.cpp
    src/squirtle/material.cpp
    src/squirtle/camera.cpp
)

set_target_properties(kame-cpp PROPERTIES
    C_STANDARD 99
    C_EXTENSIONS OFF
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    target_compile_options(kame-cpp PRIVATE /W4)
else()
    set_target_properties(kame-cpp PROPERTIES
        COMPILE_WARNING_AS_ERROR ON
    )
    target_compile_options(kame-cpp PRIVATE -Wall -Wextra -pedantic)
endif()

target_link_libraries(kame-cpp PUBLIC
    SDL3::SDL3
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    volk::volk_headers
    spirv-cross-core
    magic_enum::magic_enum
    pystring
    stb
    soloud
)

target_include_directories(kame-cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

option(KAME_BUILD_TOOLS "Build kame tools" OFF)
if (KAME_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

option(KAME_BUILD_EXAMPLES "Build kame examples" OFF)
if(KAME_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(KAME_BUILD_TESTS "Build kame tests" OFF)
if(KAME_BUILD_TESTS)
    enable_testing()

    add_executable(unit_test test/test.cpp)
    target_include_directories(unit_test PRIVATE $<BUILD_INTERFACE:${glm_SOURCE_DIR}>)
    set_target_properties(unit_test PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
    target_link_libraries(unit_test PRIVATE kame-cpp GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(unit_test)
endif()

if(KAME_BINDING_PYTHON)
    pybind11_add_module(kame bindings/python/kame-py.cpp)
    target_link_libraries(kame PRIVATE kame-cpp)
endif()

#install(TARGETS kame-cpp
#    EXPORT kame-cpp
#    LIBRARY DESTINATION lib
#    INCLUDES DESTINATION include
#)
#
#install(DIRECTORY include/kame DESTINATION include)
