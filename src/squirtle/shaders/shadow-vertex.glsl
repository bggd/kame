R"(#if __VERSION__ < 130
#define in attribute
#define out varying
#endif

in vec3 vPos;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(vPos, 1.0);
}
)"
