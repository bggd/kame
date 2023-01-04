include(FetchContent)

set(SDL_STATIC OFF CACHE INTERNAL "")
FetchContent_Declare(SDL
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.26.2/SDL2-2.26.2.tar.gz
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.11.0.tar.gz
)
FetchContent_MakeAvailable(spdlog)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
FetchContent_Declare(json
    URL URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# v1.1.4
add_library(pystring STATIC pystring/pystring.cpp)
target_include_directories(pystring PUBLIC pystring)

if(KAME_BUILD_TESTS)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(glm
        URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.7z
    )
    FetchContent_MakeAvailable(glm)
endif()
