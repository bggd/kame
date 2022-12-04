R"(#if __VERSION__ < 130
#define in varying
#define FragColor gl_FragColor
#define texture(sampler, p) texture2D(sampler, p)
#else
out vec4 FragColor;
#endif

in vec2 pUV;
uniform sampler2D diffuseTexture;

void main() {
  FragColor = texture(diffuseTexture, pUV);
}
)"
