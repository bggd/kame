add_executable(exampleKameHelloWorld
    00-helloworld/helloworld.cpp)
target_link_libraries(exampleKameHelloWorld kame-cpp)

add_executable(exampleKameTriangle
    01-triangle/triangle.cpp)
target_link_libraries(exampleKameTriangle kame-cpp)

add_executable(exampleKameImage
    02-image/image.cpp)
target_link_libraries(exampleKameImage kame-cpp)

add_executable(exampleKameGltf
    04-gltf/gltf.cpp)
target_link_libraries(exampleKameGltf kame-cpp)

set_target_properties(
    exampleKameHelloWorld
    exampleKameTriangle
    exampleKameImage exampleKameGltf
    PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF
)
