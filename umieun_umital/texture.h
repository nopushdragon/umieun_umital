#pragma once
#include "headers.h"

// --- 텍스처 정보 구조체 (MTL/FBX 재질용) ---
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
    glm::vec3 diffuseColor = glm::vec3(1.0f);   // 재질 색상 추가: 텍스처가 없을 경우 사용할 확산색

    glm::vec3 specularColor = glm::vec3(0.0f); // Ks (거울 반사 색상)
    int shininess = 1;                         // Ns (반사 강도)
};

// --- 텍스처 로딩 유틸리티 함수 ---
unsigned int loadTextureFromFile(const char* path, const std::string& directory);

struct OBB {
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 u[3];                         // 세 정규직교 u[0]x, u[1]y, u[2]z
    glm::vec3 half_length = glm::vec3(0.0f);
};
