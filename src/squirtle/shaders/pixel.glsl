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

uniform vec3 uEyePos;

vec3 phong(vec3 surfaceNormal, vec3 surfacePosition)
{
    vec3 N = normalize(surfaceNormal);
    float specularShininess = 32.0;
    vec3 result = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightNode light = uLights[i];

        vec3 lightDirection;
        float attenuation = 1.0;
        if (light.position.w > 0.5) // point light
        {
            lightDirection = normalize(light.position.xyz - surfacePosition);
            float distanceToLight = length(light.position.xyz - surfacePosition);
            attenuation = 1.0 / (light.linearAttenuation * distanceToLight + ((distanceToLight * distanceToLight) * light.quadraticAttenuation) + light.constantAttenuation);
        }
        else // directional light
        {
            lightDirection = normalize(-light.position.xyz);
        }
        float diffuse_N_dot_lightDir = max(dot(N, lightDirection), 0.0);
        vec3 diffuse = diffuse_N_dot_lightDir * light.diffuse;

        vec3 specular = vec3(0.0);
        if (diffuse_N_dot_lightDir > 0.0)
        {
            vec3 viewDirection = normalize(uEyePos - surfacePosition);
            vec3 reflectDirection = reflect(-viewDirection, N);
            specular = pow(max(dot(viewDirection, reflectDirection), 0.0), specularShininess) * light.diffuse;
        }

        vec3 ambient = light.ambient;
        diffuse *= attenuation;
        specular *= attenuation;

        result += (ambient + diffuse + specular);
    }

    return result;
}

void main() {
    FragColor = texture(diffuseTexture, pUV);
    FragColor *= vec4(phong(pNormal, pPos), 1.0);
}
)"
