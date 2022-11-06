cmake_minimum_required(VERSION 3.5)

project(kame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory(SDL)
add_subdirectory(spdlog)

add_library(kame
    STATIC
        src/kame.cpp
        src/sdl2/sdl2.cpp
	src/ogl21/glad.cpp
	src/ogl21/ogl21.cpp
	src/gltf/gltf.cpp
	pystring/pystring.cpp
)

target_link_libraries(kame PUBLIC SDL2::SDL2 SDL2::SDL2main spdlog::spdlog)

target_include_directories(kame
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SDL/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/assimp/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

add_executable(kameHelloWorld
    examples/00-helloworld/helloworld.cpp)
target_link_libraries(kameHelloWorld kame)

add_executable(kameTriangle
    examples/01-triangle/triangle.cpp)
target_link_libraries(kameTriangle kame)

add_executable(kameImage
    examples/02-image/image.cpp)
target_link_libraries(kameImage kame)

add_executable(kameGltf
    examples/04-gltf/gltf.cpp)
target_link_libraries(kameGltf kame)
