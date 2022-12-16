cmake_minimum_required(VERSION 3.5)

project(kame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory(thirdparty/SDL)
add_subdirectory(thirdparty/spdlog)

add_library(kame
    STATIC
        src/kame.cpp
        src/math/vector3f.cpp
        src/math/vector4f.cpp
        src/math/matrix4x4f.cpp
        src/math/quaternion.cpp
        src/sdl/sdl.cpp
	src/ogl21/glad.cpp
	src/ogl21/ogl21.cpp
	src/gltf/gltf.cpp
        src/squirtle/engine.cpp
        src/squirtle/node/helper.cpp
        thirdparty/pystring/pystring.cpp
)

target_link_libraries(kame PUBLIC SDL2::SDL2 SDL2::SDL2main spdlog::spdlog)

target_include_directories(kame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

add_executable(exampleKameHelloWorld
    examples/00-helloworld/helloworld.cpp)
target_link_libraries(exampleKameHelloWorld kame)

add_executable(exampleKameTriangle
    examples/01-triangle/triangle.cpp)
target_link_libraries(exampleKameTriangle kame)

add_executable(exampleKameImage
    examples/02-image/image.cpp)
target_link_libraries(exampleKameImage kame)

add_executable(exampleKameGltf
    examples/04-gltf/gltf.cpp)
target_link_libraries(exampleKameGltf kame)


add_executable(squirtleEditor
    src/squirtle/editor/editor.cpp
)

include(ExternalProject)
ExternalProject_Add(
    FLTK14
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/fltk
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE:STRINGS=${CMAKE_CONFIGURATION_TYPES}
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
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_directories(squirtleEditor PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib)
target_link_libraries(squirtleEditor PRIVATE kame gdiplus Ws2_32 comctl32 fltk)


enable_testing()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/googletest)

add_executable(unit_test test/test.cpp)
target_link_libraries(unit_test PRIVATE kame GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(unit_test)

