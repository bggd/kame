cmake_minimum_required(VERSION 3.11)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

project(kame-proj LANGUAGES C CXX)

add_subdirectory(thirdparty)

add_library(kame-cpp
    src/kame.cpp
    src/math/vector3.cpp
    src/math/vector4.cpp
    src/math/matrix.cpp
    src/math/quaternion.cpp
    src/sdl/sdl.cpp
    src/ogl/glad.cpp
    src/ogl/ogl.cpp
    src/gltf/gltf.cpp
    src/gltf/gltf_material.cpp
    src/gltf/gltf_ext.cpp
    src/squirtle/squirtle.cpp
    src/squirtle/instance.cpp
    src/squirtle/animation.cpp
    src/squirtle/material.cpp
    src/squirtle/camera.cpp
    src/lua/lua.cpp
    src/love2d/love2d.cpp
    src/love2d/detail/context.cpp
    src/love2d/detail/graphics.cpp
    src/love2d/detail/renderer.cpp
    src/love2d/detail/physics.cpp
    src/love2d/detail/physics_debug_draw.cpp
    src/love2d/detail/box2d.cpp
    src/love2d/graphics.cpp
    src/love2d/physics.cpp
    src/love2d/timer.cpp
    src/love2d/mouse.cpp
    src/love2d/keyboard.cpp
)

set_target_properties(kame-cpp PROPERTIES
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

target_link_libraries(kame-cpp PUBLIC SDL3::SDL3 spdlog::spdlog nlohmann_json::nlohmann_json pystring box2d stb soloud)

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

option(KAME_BINDING_LUA "Build lua binding" OFF)
if(KAME_BINDING_LUA)
    add_library(kame SHARED
        bindings/lua/kame-lua.cpp
    )

    set_target_properties(kame PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""
    )

    target_include_directories(kame-cpp PUBLIC ${KAME_LUA_INCDIR})
    target_include_directories(kame PUBLIC ${KAME_LUA_INCDIR})

    target_link_libraries(kame PUBLIC kame-cpp ${KAME_LUALIB})

    install(TARGETS kame
        EXPORT kame
        RUNTIME DESTINATION lib
        LIBRARY DESTINATION lib
    )
else()
    target_link_libraries(kame-cpp PUBLIC lua)
endif()

#install(TARGETS kame-cpp
#    EXPORT kame-cpp
#    LIBRARY DESTINATION lib
#    INCLUDES DESTINATION include
#)
#
#install(DIRECTORY include/kame DESTINATION include)
