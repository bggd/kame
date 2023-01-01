cmake_minimum_required(VERSION 3.11)

project(kame LANGUAGES C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

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
        src/squirtle/node/gltf_node.cpp
        src/squirtle/engine.cpp
        src/squirtle/node/helper.cpp
)
set_target_properties(kame PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

target_link_libraries(kame PUBLIC SDL2::SDL2 spdlog::spdlog nlohmann_json::nlohmann_json pystring)

target_include_directories(kame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)


option(KAME_BUILD_EXAMPLES "Build kame examples" ON)
if(KAME_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(KAME_BUILD_SQUIRTLE_EDITOR "Build kame::Squirtle Editor" OFF)
if(KAME_BUILD_SQUIRTLE_EDITOR)
    add_executable(squirtleEditor
        src/squirtle/editor/editor.cpp
    )
    set_target_properties(squirtleEditor PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

    include(ExternalProject)
    ExternalProject_Add(
        FLTK14
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/fltk
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/fltk_install
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DFLTK_BUILD_TEST:BOOL=OFF
            -DFLTK_BUILD_FLUID:BOOL=OFF
            -DOPTION_USE_SYSTEM_LIBJPEG:BOOL=OFF
            -DOPTION_USE_SYSTEM_LIBPNG:BOOL=OFF
            -DOPTION_USE_SYSTEM_ZLIB:BOOL=OFF
            -DOPTION_BUILD_HTML_DOCUMENTATION:BOOL=OFF
            -DOPTION_BUILD_PDF_DOCUMENTATION:BOOL=OFF
    )
    add_dependencies(squirtleEditor FLTK14)


    target_include_directories(squirtleEditor
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/fltk_install/include
    )
    target_link_directories(squirtleEditor PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/fltk_install/lib)

    target_link_libraries(squirtleEditor PRIVATE kame gdiplus Ws2_32 comctl32 fltk$<$<CONFIG:Debug>:d>)
endif()

option(KAME_BUILD_TESTS "Build kame tests" ON)
if(KAME_BUILD_TESTS)
    enable_testing()

    add_executable(unit_test test/test.cpp)
    target_include_directories(unit_test PRIVATE $<BUILD_INTERFACE:${glm_SOURCE_DIR}>)
    set_target_properties(unit_test PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
    target_link_libraries(unit_test PRIVATE kame GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(unit_test)
endif()

