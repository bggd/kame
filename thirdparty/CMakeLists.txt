include(FetchContent)

set(SDL_STATIC OFF CACHE INTERNAL "")
FetchContent_Declare(SDL
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.26.4/SDL2-2.26.4.tar.gz
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.11.0.tar.gz
)
FetchContent_MakeAvailable(spdlog)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(lua
    URL https://github.com/lua/lua/archive/refs/tags/v5.4.4.tar.gz
)
FetchContent_MakeAvailable(lua)
add_library(lua STATIC
    ${lua_SOURCE_DIR}/lzio.c
    ${lua_SOURCE_DIR}/lctype.c
    ${lua_SOURCE_DIR}/lopcodes.c
    ${lua_SOURCE_DIR}/lmem.c
    ${lua_SOURCE_DIR}/lundump.c
    ${lua_SOURCE_DIR}/ldump.c
    ${lua_SOURCE_DIR}/lstate.c
    ${lua_SOURCE_DIR}/lgc.c
    ${lua_SOURCE_DIR}/llex.c
    ${lua_SOURCE_DIR}/lcode.c
    ${lua_SOURCE_DIR}/lparser.c
    ${lua_SOURCE_DIR}/ldebug.c
    ${lua_SOURCE_DIR}/lfunc.c
    ${lua_SOURCE_DIR}/lobject.c
    ${lua_SOURCE_DIR}/ltm.c
    ${lua_SOURCE_DIR}/lstring.c
    ${lua_SOURCE_DIR}/ltable.c
    ${lua_SOURCE_DIR}/ldo.c
    ${lua_SOURCE_DIR}/lvm.c
    ${lua_SOURCE_DIR}/lapi.c

    ${lua_SOURCE_DIR}/lauxlib.c

    ${lua_SOURCE_DIR}/lbaselib.c
    ${lua_SOURCE_DIR}/lcorolib.c
    ${lua_SOURCE_DIR}/ldblib.c
    ${lua_SOURCE_DIR}/liolib.c
    ${lua_SOURCE_DIR}/lmathlib.c
    ${lua_SOURCE_DIR}/loadlib.c
    ${lua_SOURCE_DIR}/loslib.c
    ${lua_SOURCE_DIR}/lstrlib.c
    ${lua_SOURCE_DIR}/ltablib.c
    ${lua_SOURCE_DIR}/lutf8lib.c
    ${lua_SOURCE_DIR}/linit.c
)
target_include_directories(lua PUBLIC ${lua_SOURCE_DIR})

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
