#version 330 core


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 materialColorDefault;
uniform bool bUseTexture;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 viewPos;

uniform vec3 materialSpecular;
uniform int shininess;

uniform vec3 u_FogColor;
uniform float u_FogStart;
uniform float u_FogEnd;

uniform float fogEnabled;

uniform bool flashlight_on;
uniform vec3 flashPos;
uniform vec3 flashDir;
uniform float flashCutOff;
uniform float flashOuterCutOff;


void main()
{
    vec3 materialDiffuse;
    if (bUseTexture) {
        materialDiffuse = texture(texture_diffuse1, TexCoords).rgb;
    }
    else {
        materialDiffuse = materialColorDefault;
    }

    // 법선 정규화
    vec3 norm = Normal;
    float normLength = length(norm);
    if (normLength > 0.0001) {
        norm = norm / normLength;
    } else {
        norm = vec3(0.0, 1.0, 0.0);
    }
    
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Ambient
    vec3 ambient = ambientStrength * lightColor * materialDiffuse;
    
    // Diffuse
    float NdotL = dot(norm, lightDir);
    float diffuseFactor = max(NdotL, 0.0);
    
    // 너무 밝아지는 것 방지
    diffuseFactor = min(diffuseFactor, 0.8);  // 최대 0.8
    
    vec3 diffuse = diffuseFactor * lightColor * materialDiffuse;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), float(shininess));
    
    if (isnan(spec) || isinf(spec)) {
        spec = 0.0;
    }
    
    vec3 specular = spec * lightColor * (materialSpecular * 0.05);

    // 최종 결과
    vec3 result = ambient + diffuse + specular;

    if (flashlight_on) {
        vec3 flashLightColor = vec3(1.0, 1.0, 1.0);
		vec3 lightDirFlash = normalize(flashPos - FragPos);
		float theta = dot(lightDirFlash, normalize(-flashDir));
		float epsilon = flashCutOff - flashOuterCutOff;
		float intensity = clamp((theta - flashOuterCutOff) / epsilon, 0.0, 1.0);
		float distance = length(flashPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

        float diffFlash = max(dot(norm, lightDirFlash), 0.0);
        vec3 diffuseFlash = diffFlash * flashLightColor * materialDiffuse;


        vec3 reflectDirFlash = reflect(-lightDirFlash, norm);
        float specFlash = pow(max(dot(viewDir, reflectDirFlash), 0.0), float(shininess));
        vec3 specularFlash = specFlash * flashLightColor * (materialSpecular * 0.5);

 
        diffuseFlash *= attenuation * intensity;
        specularFlash *= attenuation * intensity;


        result += (diffuseFlash + specularFlash);
    }
    
    // 최종 밝기도 제한 (추가 안전장치)
    result = min(result, vec3(1.0));

    if (fogEnabled > 0.5) {
        float dist = distance(viewPos, FragPos);
        float fogFactor = (u_FogEnd - dist) / (u_FogEnd - u_FogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        result = mix(u_FogColor, result, fogFactor);
    }
    
    FragColor = vec4(result, 1.0);
}