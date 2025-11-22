#include "animated_model.h"

// ===================================================================
// AnimatedMesh
// ===================================================================
void AnimatedMesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AnimatedVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 위치(0), 법선(1), 텍스처 좌표(2) 설정 (StaticVertex와 동일)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, TexCoords));

    // 뼈대 인덱스 속성 (layout=3, 정수 벡터)
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, BoneIDs));

    // 가중치 속성 (layout=4, 실수 벡터)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Weights));

    glBindVertexArray(0);
}

void AnimatedMesh::Draw(GLuint shaderID) {
    // 텍스처 바인딩 로직: textures 벡터를 순회하며 glActiveTexture, glBindTexture 호출
    // ... (구현 필요)

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ===================================================================
// AnimatedModel
// ===================================================================
void AnimatedModel::processNode(aiNode* node, const aiScene* scene) {

}

AnimatedMesh AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	return AnimatedMesh();
}

void AnimatedModel::extractBoneWeight(AnimatedMesh& mesh, aiMesh* aiMesh) {
}

AnimatedModel::AnimatedModel(const std::string& fbxPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fbxPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // 애니메이션 관련 초기화 로직: 뼈대 계층 구조 파싱 및 finalBoneMatrices 벡터 크기 조정
    // ... (구현 필요)
    finalBoneMatrices.resize(MAX_BONES, glm::mat4(1.0f));

    processNode(scene->mRootNode, scene);
}

void AnimatedModel::updateAnimation(float time) {
    // 애니메이션 재생 로직: 시간(time)을 기준으로 뼈 행렬(finalBoneMatrices)을 계산
    // Assimp의 애니메이션, 노드 변환, 키프레임 보간 로직 포함
    // ... (매우 복잡, 구현 필요)
}

void AnimatedModel::Draw(GLuint shaderID) {
    // 최종 뼈대 행렬을 셰이더 유니폼으로 전송
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uFinalBoneMatrices"),
        finalBoneMatrices.size(), GL_FALSE, glm::value_ptr(finalBoneMatrices[0]));

    for (auto& mesh : meshes) mesh.Draw(shaderID);
}