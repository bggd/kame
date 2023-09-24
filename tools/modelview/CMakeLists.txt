add_executable(modelview
    main.cpp
)
set_target_properties(modelview PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
target_include_directories(modelview PRIVATE imgui)
target_link_libraries(modelview PUBLIC kame imgui)
