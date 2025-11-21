#version 330 core
#define MAX_BONE_PER_VERTEX 4
#define MAX_BONES 100 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;   // »À´ë ÀÎµ¦½º
layout (location = 4) in vec4 aWeights;    // °¡ÁßÄ¡

uniform mat4 uFinalBoneMatrices[MAX_BONES]; 

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

void main()
{
    // »À´ë º¯È¯ °è»ê
    mat4 boneTransform = mat4(0.0);
    for(int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        boneTransform += uFinalBoneMatrices[aBoneIDs[i]] * aWeights[i]; 
    }
    
    mat4 finalModelMatrix = uModel * boneTransform;

    gl_Position = uProj * uView * finalModelMatrix * vec4(aPos, 1.0);
    FragPos = vec3(finalModelMatrix * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(finalModelMatrix))) * aNormal; 
    TexCoords = aTexCoords;
}