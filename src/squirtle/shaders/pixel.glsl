R"(#if __VERSION__ < 130
#define in varying
#define FragColor gl_FragColor
#define texture(sampler, p) texture2D(sampler, p)
#else
out vec4 FragColor;
#endif

in vec2 pUV;
in vec3 pNormal;
in vec3 pPos;

uniform sampler2D diffuseTexture;

struct LightNode {
    vec4 position;
    vec3 ambient;
    vec3 diffuse;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
uniform LightNode uLights[MAX_LIGHTS];

vec3 phong(vec3 surfaceNormal, vec3 surfacePosition)
{
    vec3 N = normalize(surfaceNormal);
    vec3 result = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightNode light = uLights[i];

        vec3 L = vec3(0.0);
        float attenuation = 1.0;
        if (light.position.w > 0.5) {
            // point light
            L = normalize(light.position.xyz - surfacePosition);
            float distance = length(light.position.xyz - surfacePosition);
            attenuation = 1.0 / (light.linearAttenuation * distance + ((distance * distance) * light.quadraticAttenuation) + light.constantAttenuation);
        }
        else {
            // directional light
            L = -normalize(light.position.xyz);
        }
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = vec3(NdotL) * light.diffuse;

        vec3 specular = vec3(0.0);
        if (NdotL > 0.0) {
            vec3 V = -normalize(surfacePosition);
            vec3 R = normalize(reflect(-L, N));
            specular = light.diffuse * pow(max(dot(R, V), 0.0), 8.0);
        }

        diffuse *= attenuation;
        specular *= attenuation;

        result += light.ambient + diffuse + specular;
    }

    return result;
}

void main() {
    FragColor = texture(diffuseTexture, pUV);
    FragColor *= vec4(phong(pNormal, pPos), 1.0);
}
)"
