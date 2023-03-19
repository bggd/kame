add_executable(matc
    main.cpp
)
set_target_properties(matc PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
target_include_directories(matc PRIVATE glslang spirv-cross)
target_link_libraries(matc PUBLIC kame glslang glslang-default-resource-limits SPIRV spirv-cross-c spdlog)
