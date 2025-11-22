#pragma once
#include "headers.h"

struct AnimatedVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    int BoneIDs[MAX_BONE_PER_VERTEX];
    float Weights[MAX_BONE_PER_VERTEX];
};

struct AnimatedMesh {
    std::vector<AnimatedVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO, VBO, EBO;

    void setupMesh();

    void Draw(GLuint shaderID);
};

class AnimatedModel {
private:
    void processNode(aiNode* node, const aiScene* scene);
    AnimatedMesh processMesh(aiMesh* mesh, const aiScene* scene);
    // 뼈대 정보 로딩 함수 (FBX 파싱 시 호출)
    void extractBoneWeight(AnimatedMesh& mesh, aiMesh* aiMesh);
    // 텍스처 로딩 유틸리티 (구현 필요)

public:
    std::vector<AnimatedMesh> meshes;
    std::map<std::string, BoneInfo> boneInfoMap; // 뼈 이름과 정보 매핑
    std::vector<glm::mat4> finalBoneMatrices; // 매 프레임 계산된 뼈대 행렬
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    int boneCounter = 0;

    AnimatedModel(const std::string& fbxPath);

    void updateAnimation(float time);

    void Draw(GLuint shaderID);
};

