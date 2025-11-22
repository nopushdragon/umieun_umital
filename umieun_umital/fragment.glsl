#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture_diffuse1; // <-- fbx용 텍스처
uniform vec3 materialColorDefault; // <-- 얜 mtl
uniform bool bUseTexture;          // <-- 텍스쳐 할건지 mtl할건지

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 viewPos;

uniform vec3 materialSpecular;
uniform int shininess;

void main()
{
    // 텍스처에서 Diffuse 색상 샘플링 
    vec3 materialDiffuse;
    if (bUseTexture) {
        // 텍스처를 사용하도록 플래그가 설정된 경우
        materialDiffuse = texture(texture_diffuse1, TexCoords).rgb;
    }
    else {
        // 텍스처가 없는 경우, C++에서 전달한 기본 색상(Kd)을 사용
        materialDiffuse = materialColorDefault;
    }

    // 1. 주변 조명 (Ambient)
    vec3 ambient = ambientStrength * lightColor * materialDiffuse;

    // 2. 확산 조명 (Diffuse)
    vec3 normalVector = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffuseFactor = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseFactor * lightColor * materialDiffuse;

    // 3. 거울 반사 조명 (Specular)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalVector);
    float specularFactor = max(dot(viewDir, reflectDir), 0.0);
    specularFactor = pow(specularFactor, shininess);
    vec3 specular = specularFactor * lightColor * materialSpecular;

    // 4. 최종 색상 합산
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}