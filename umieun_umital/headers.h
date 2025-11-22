#pragma once
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <string>
#include <deque>
#include <utility>
#include <vector>
#include <fstream>
#include <random>
#include <sstream>
#include <map>
#include <string_view>
#include "fmod.hpp"
#include <thread>
#include <chrono>
#include <cctype>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp> // 쿼터니언 필수
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include <glm/ext.hpp>
#include <ft2build.h>
#include "stb_image.h"
#include FT_FREETYPE_H
using namespace std;


// ===================================================================
// 상수 및 전역 변수 정의
// ===================================================================
#define MAX_BONE_PER_VERTEX 4
#define MAX_BONES           100
#define WINDOW_WIDTH        1200
#define WINDOW_HEIGHT       800

// --- 텍스처 로딩 유틸리티 함수 ---
unsigned int loadTextureFromFile(const char* path, const std::string& directory);

// --- 뼈대 정보 구조체 (FBX 애니메이션용) ---
struct BoneInfo {
    int id;
    glm::mat4 offset; // Assimp의 오프셋 행렬을 GLM 행렬로 변환해야 함
};
void aiMatrix4x4ToGlm(const aiMatrix4x4* from, glm::mat4& to); // Assimp 행렬을 GLM으로 변환하는 함수 (구현 필요)

// --- 텍스처 정보 구조체 (MTL/FBX 재질용) ---
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
    glm::vec3 diffuseColor = glm::vec3(1.0f);   // 재질 색상 추가: 텍스처가 없을 경우 사용할 확산색

    // 텍스처 파일 로딩 함수 (구현 필요)
};