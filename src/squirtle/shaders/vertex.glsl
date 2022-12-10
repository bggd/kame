R"(#if __VERSION__ < 130
#define in attribute
#define out varying
#endif

in vec3 vPos;
in vec2 vUV;
in vec3 vNormal;

out vec2 pUV;
out vec3 pNormal;
out vec3 pPos;
out vec4 pShadowCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uShadowMVP;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(vPos, 1.0);
    pUV = vUV;
    pNormal = transpose(inverse(mat3(uModel))) * vNormal;
    pPos = (uModel * vec4(vPos, 1.0)).xyz;
    pShadowCoord = uShadowMVP * vec4(vPos, 1.0);
}
)"
