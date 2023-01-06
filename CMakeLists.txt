cmake_minimum_required(VERSION 3.11)

project(kame LANGUAGES C CXX)

option(KAME_BUILD_TESTS "Build kame tests" OFF)

add_subdirectory(thirdparty)

add_library(kame
    STATIC
        src/kame.cpp
        src/math/vector3f.cpp
        src/math/vector4f.cpp
        src/math/matrix4x4f.cpp
        src/math/quaternion.cpp
        src/sdl/sdl.cpp
        src/ogl/glad.cpp
        src/ogl/ogl.cpp
        src/gltf/gltf.cpp
        src/lua/lua.cpp
)
set_target_properties(kame PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

target_link_libraries(kame PUBLIC SDL2::SDL2 SDL2::SDL2main spdlog::spdlog nlohmann_json::nlohmann_json pystring lua)

target_include_directories(kame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)


option(KAME_BUILD_EXAMPLES "Build kame examples" OFF)
if(KAME_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(KAME_BUILD_TESTS)
    enable_testing()

    add_executable(unit_test test/test.cpp)
    target_include_directories(unit_test PRIVATE $<BUILD_INTERFACE:${glm_SOURCE_DIR}>)
    set_target_properties(unit_test PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
    target_link_libraries(unit_test PRIVATE kame GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(unit_test)
endif()

