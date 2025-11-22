#pragma once
#include "headers.h"
#include "static_model.h"

// --- 미로 배치 인스턴스 ---
struct MazeBlockInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
    glm::vec3 reset;    // 미로 초기 위치 저장용
};
extern std::vector<MazeBlockInstance> mazeBlocks;

void printMaze();

void generateMaze(int cx, int cy);
void generatetype();
void initmaze(std::vector<StaticModel*>);

void setMaze();
