#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    // 반사(reflect) 같은 계산 없이 텍스처 색상을 그대로 출력
    FragColor = texture(skybox, normalize(TexCoords));
}