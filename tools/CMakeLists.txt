add_subdirectory(uvview)
add_subdirectory(modelview)


set(BUILD_EXTERNAL OFF CACHE INTERNAL "")
set(ENABLE_CTEST OFF CACHE INTERNAL "")
FetchContent_Declare(glslang
    URL https://github.com/KhronosGroup/glslang/archive/refs/tags/12.0.0.tar.gz
)
FetchContent_MakeAvailable(glslang)


set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE INTERNAL "")
FetchContent_Declare(spirvcross
    URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/sdk-1.3.239.0.tar.gz
)
FetchContent_MakeAvailable(spirvcross)


add_subdirectory(matc)
