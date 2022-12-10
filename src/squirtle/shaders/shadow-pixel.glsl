R"(#if __VERSION__ < 130
#define in varying
#define FragColor gl_FragColor
#define FragDepth gl_FragDepth
#define texture(sampler, p) texture2D(sampler, p)
#else
out vec4 FragColor;
out float FragDepth;
#endif

void main()
{
}
)"
