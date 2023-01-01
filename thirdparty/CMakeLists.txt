include(FetchContent)

FetchContent_Declare(SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.26.1
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.11.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

FetchContent_Populate(pystring
    GIT_REPOSITORY https://github.com/imageworks/pystring.git
    GIT_TAG v1.1.4
)
add_library(pystring STATIC ${pystring_SOURCE_DIR}/pystring.cpp)
target_include_directories(pystring PUBLIC ${pystring_SOURCE_DIR})

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)
