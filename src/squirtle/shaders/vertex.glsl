R"(#if __VERSION__ < 130
#define in attribute
#define out varying
#endif

in vec3 vPos;
in vec2 vUV;

out vec2 pUV;

uniform mat4 uModelViewProjection;

void main() {
  gl_Position = uModelViewProjection * vec4(vPos.x, vPos.y, vPos.z, 1.0);
  pUV = vUV;
}
)"
