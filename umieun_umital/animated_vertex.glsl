#version 330 core
#define MAX_BONE_PER_VERTEX 4
#define MAX_BONES 100 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;   // 뼈대 인덱스
layout (location = 4) in vec4 aWeights;    // 가중치

uniform mat4 uFinalBoneMatrices[MAX_BONES]; 

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uModelNormal;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

void main()
{
    mat4 BoneTransform = mat4(0.0f);
    float totalWeight = 0.0f;

    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        // [수정 3] 정수형이므로 -1과 비교
        if (aBoneIDs[i] == -1)
            continue;

        if (aBoneIDs[i] >= MAX_BONES)
            break;

        // [수정 4] 배열 인덱스에 int() 캐스팅 불필요 (이미 정수임)
        BoneTransform += uFinalBoneMatrices[aBoneIDs[i]] * aWeights[i];
        totalWeight += aWeights[i];
    }

    // 가중치가 없으면 기본 행렬 사용
    if (totalWeight == 0.0f) {
        BoneTransform = mat4(1.0f);
    }

    vec4 animatedPos = BoneTransform * vec4(aPos, 1.0f);
    FragPos = vec3(uModel * animatedPos);

    vec3 animatedNormal = mat3(BoneTransform) * aNormal;
    Normal = uModelNormal * animatedNormal;

    TexCoords = aTexCoords;
    gl_Position = uProj * uView * vec4(FragPos, 1.0);
}