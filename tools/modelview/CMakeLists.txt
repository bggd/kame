add_executable(modelview
    main.cpp
)
set_target_properties(modelview PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
target_link_libraries(modelview PUBLIC kame)
