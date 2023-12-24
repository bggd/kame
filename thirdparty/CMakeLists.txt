include(FetchContent)

# Dec 8, 2023
FetchContent_Declare(SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG 3c5e9e61123f9e020081a4a7936e305dc1ef378a
)
set(SDL_SHARED_DEFAULT OFF CACHE INTERNAL "")
set(SDL_SHARED OFF CACHE INTERNAL "")
set(SDL_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.tar.gz
)
FetchContent_MakeAvailable(spdlog)


FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

FetchContent_Declare(volk
    URL https://github.com/zeux/volk/archive/refs/tags/1.3.270.tar.gz
)
set(VOLK_HEADERS_ONLY ON CACHE INTERNAL "")
FetchContent_MakeAvailable(volk)

FetchContent_Declare(magic_enum
    URL https://github.com/Neargye/magic_enum/archive/refs/tags/v0.9.5.tar.gz
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(lua
    URL https://github.com/lua/lua/archive/refs/tags/v5.4.6.tar.gz
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

FetchContent_Declare(imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.tar.gz
)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

FetchContent_Declare(box2d
    URL https://github.com/erincatto/box2d/archive/refs/tags/v2.4.1.tar.gz
)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(box2d)

# Oct 12, 2023
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG beebb24b945efdea3b9bba23affb8eb3ba8982e7
)
FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

FetchContent_Declare(soloud
    URL https://github.com/jarikomppa/soloud/archive/refs/tags/RELEASE_20200207.tar.gz
)
FetchContent_MakeAvailable(soloud)
add_library(soloud STATIC
    ${soloud_SOURCE_DIR}/src/core/soloud.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_audiosource.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_bus.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_3d.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_basicops.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_faderops.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_filterops.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_getters.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_setters.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_voicegroup.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_core_voiceops.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_fader.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_fft.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_fft_lut.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_file.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_filter.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_misc.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_queue.cpp
    ${soloud_SOURCE_DIR}/src/core/soloud_thread.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/monotone/soloud_monotone.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/noise/soloud_noise.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/openmpt/soloud_openmpt.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/openmpt/soloud_openmpt_dll.c

    ${soloud_SOURCE_DIR}/src/audiosource/sfxr/soloud_sfxr.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/speech/darray.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/speech/klatt.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/speech/resonator.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/speech/soloud_speech.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/speech/tts.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/tedsid/sid.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/tedsid/soloud_tedsid.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/tedsid/ted.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/vic/soloud_vic.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/vizsn/soloud_vizsn.cpp

    ${soloud_SOURCE_DIR}/src/audiosource/wav/dr_impl.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/wav/soloud_wav.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/wav/soloud_wavstream.cpp
    ${soloud_SOURCE_DIR}/src/audiosource/wav/stb_vorbis.c

    ${soloud_SOURCE_DIR}/src/backend/miniaudio/soloud_miniaudio.cpp
)
target_compile_definitions(soloud PRIVATE WITH_MINIAUDIO=1)
target_include_directories(soloud
    PUBLIC
        ${soloud_SOURCE_DIR}/include
    PRIVATE
        ${soloud_SOURCE_DIR}/src/audiosource/speech
        ${soloud_SOURCE_DIR}/src/audiosource/tedsid
        ${soloud_SOURCE_DIR}/src/audiosource/wav

        ${soloud_SOURCE_DIR}/src/backend/miniaudio
)

if(KAME_BUILD_TESTS)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(glm
        URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.7z
    )
    FetchContent_MakeAvailable(glm)
endif()
