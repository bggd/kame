cmake_minimum_required(VERSION 3.5)

project(kame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory(SDL)
add_subdirectory(spdlog)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT  OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)

add_library(kame
    STATIC
	src/kame.cpp
    src/sdl2/sdl2.cpp
	src/ogl21/glad.cpp
	src/ogl21/ogl21.cpp
	src/assimp/assimp.cpp
)

target_link_libraries(kame PUBLIC SDL2::SDL2 SDL2::SDL2main spdlog::spdlog assimp)

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
add_executable(kameAssimp
examples/03-assimp/assimp.cpp)
target_link_libraries(kameAssimp kame)
