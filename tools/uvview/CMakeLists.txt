add_executable(uvview
    main.cpp
)
set_target_properties(uvview PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
target_link_libraries(uvview PUBLIC kame-cpp)
