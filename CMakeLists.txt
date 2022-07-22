cmake_minimum_required(VERSION 3.5)

project(kame LANGUAGES CXX)

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
)

target_link_libraries(kame PRIVATE SDL2::SDL2 SDL2::SDL2main spdlog::spdlog)

target_include_directories(kame
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SDL/include>
    PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

add_executable(kameExe
main.cpp)
target_link_libraries(kameExe kame)