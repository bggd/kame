#include <kame/kame.hpp>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>
#include <spirv_cross_c.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdio>

std::string loadFile(const std::filesystem::path& path)
{
    std::string str;
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ios_base::in);
    assert(ifs.is_open());
    std::string tmp;
    while (std::getline(ifs, tmp))
    {
        str += tmp + '\n';
    }
    ifs.close();
    return str;
}

std::vector<uint32_t> glsl2spirv(const char* code, glslang_stage_t stage, const char* path)
{
    glslang_initialize_process();
    glslang_input_t input;
    input.language = GLSLANG_SOURCE_GLSL;
    input.stage = stage;
    input.client = GLSLANG_CLIENT_OPENGL;
    input.client_version = GLSLANG_TARGET_OPENGL_450;
    input.target_language = GLSLANG_TARGET_SPV;
    input.target_language_version = GLSLANG_TARGET_SPV_1_6;
    input.code = code;
    input.default_version = 330;
    input.default_profile = GLSLANG_NO_PROFILE;
    input.force_default_version_and_profile = false;
    input.forward_compatible = false;
    input.messages = GLSLANG_MSG_DEFAULT_BIT;
    input.resource = glslang_default_resource();

    glslang_shader_t* shader = glslang_shader_create(&input);
    glslang_shader_set_options(shader, GLSLANG_SHADER_AUTO_MAP_LOCATIONS);

    if (!glslang_shader_preprocess(shader, &input))
    {
        fprintf(stderr, "GLSL preprocessing failed %s\n", path);
        fprintf(stderr, "%s\n", glslang_shader_get_info_log(shader));
        fprintf(stderr, "%s\n", glslang_shader_get_info_debug_log(shader));
        fprintf(stderr, "%s\n", input.code);
        abort();
    }

    if (!glslang_shader_parse(shader, &input))
    {
        fprintf(stderr, "GLSL parsing failed %s\n", path);
        fprintf(stderr, "%s\n", glslang_shader_get_info_log(shader));
        fprintf(stderr, "%s\n", glslang_shader_get_info_debug_log(shader));
        fprintf(stderr, "%s\n", glslang_shader_get_preprocessed_code(shader));
        abort();
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        fprintf(stderr, "GLSL linking failed %s\n", path);
        fprintf(stderr, "%s\n", glslang_program_get_info_log(program));
        fprintf(stderr, "%s\n", glslang_program_get_info_debug_log(program));
        abort();
    }

    glslang_program_SPIRV_generate(program, stage);

    std::vector<uint32_t> outShaderModule(glslang_program_SPIRV_get_size(program));
    glslang_program_SPIRV_get(program, outShaderModule.data());

    const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
    if (spirv_messages)
    {
        fprintf(stderr, "(%s) %s\b", path, spirv_messages);
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);

    glslang_finalize_process();

    return outShaderModule;
}

std::string spirv2glsl(const std::vector<uint32_t>& shaderModule, uint32_t glslVersion)
{
    spvc_context context = NULL;
    spvc_parsed_ir ir = NULL;
    spvc_compiler compiler_glsl = NULL;
    spvc_compiler_options options = NULL;
    const char* result = NULL;

    spvc_context_create(&context);

    spvc_context_set_error_callback(
        context, [](void*, const char* errorMsg) {
            fprintf(stderr, "%s\n", errorMsg);
        },
        nullptr);

    spvc_context_parse_spirv(context, shaderModule.data(), shaderModule.size(), &ir);
    spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);

    spvc_compiler_create_compiler_options(compiler_glsl, &options);
    spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, glslVersion);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_FALSE);
    spvc_compiler_install_compiler_options(compiler_glsl, options);

    spvc_compiler_compile(compiler_glsl, &result);
    std::string out(result);

    spvc_context_destroy(context);

    return out;
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "matc: input.lua <output.lua>\n");
        return 1;
    }

    std::vector<std::string> args(argv, argv + argc);

    kame::lua::Lua lua;
    lua.initLua();
    if (LUA_OK != lua.doFile(argv[1]))
    {
        return 1;
    }

    std::filesystem::path basePath(argv[1]);
    std::filesystem::path vertexPath, fragmentPath;
    basePath = basePath.parent_path();
    std::string glslVertexCode;
    std::string glslFragmentCode;

    if (lua.getGlobal("glsl_vertex") == LUA_TSTRING)
    {
        vertexPath = basePath;
        vertexPath.append(lua.popString());
        glslVertexCode = loadFile(vertexPath);
    }
    else
    {
        fprintf(stderr, "matc: Vertex Shader not found");
        abort();
    }
    if (lua.getGlobal("glsl_fragment") == LUA_TSTRING)
    {
        fragmentPath = basePath;
        fragmentPath.append(lua.popString());
        glslFragmentCode = loadFile(fragmentPath);
    }
    else
    {
        fprintf(stderr, "matc: Fragment Shader not found");
        abort();
    }
    lua.shutdownLua();

    std::string ogl21;
    std::vector<uint32_t> vertexModule = glsl2spirv(glslVertexCode.c_str(), GLSLANG_STAGE_VERTEX, vertexPath.string().c_str());
    std::string result = spirv2glsl(vertexModule, 120);

    ogl21 += "{version = 120,\n";
    ogl21 += "vertex_code = [[" + result + "]],\n";

    std::vector<uint32_t> fragmentModule = glsl2spirv(glslFragmentCode.c_str(), GLSLANG_STAGE_FRAGMENT, fragmentPath.string().c_str());
    result = spirv2glsl(fragmentModule, 120);

    ogl21 += "fragment_code = [[" + result + "]]},\n";

    std::string ogl33;
    result = spirv2glsl(vertexModule, 330);

    ogl33 += "{version = 330,\n";
    ogl33 += "vertex_code = [[" + result + "]],\n";

    result = spirv2glsl(fragmentModule, 330);

    ogl33 += "fragment_code = [[" + result + "]]}\n";

    if (argc == 2)
    {
        std::cout << loadFile(std::filesystem::path(argv[1])) << '\n';
        std::cout << "glsl_shaders = {\n";
        std::cout << ogl21 << ogl33 << "}\n";
    }
    else if (argc == 3)
    {
        std::ofstream ofs;
        ofs.open(argv[2], std::ios_base::out);
        ofs << loadFile(std::filesystem::path(argv[1])) << '\n';
        ofs << "glsl_shaders = {\n";
        ofs << ogl21 << ogl33 << "}\n";
        ofs.close();
    }

    return 0;
}
