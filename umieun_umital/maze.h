#pragma once
#include "headers.h"
#include "static_model.h"
#include "resource.h"

enum PATH_WALL {
    PATH = 0,
    WALL = 1
};

struct MAZE_mem {
    int path_wall = WALL; // 0: path, 1: wall
    int type = 15;      // 0동 1서 2남 3북  4ㅡ 5ㅣ  6┌ 7┐ 8└ 9┘  10ㅏ 11ㅓ 12ㅜ 13ㅗ  14+ 15x
};

// --- 미로 배치 인스턴스 ---
struct MazeBlockInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
    glm::vec3 reset;    // 미로 초기 위치 저장용

    OBB road_world_obb;
    std::vector<OBB> obstacle_world_obb;
    bool is_colliding = false;  // 캐릭터와 충돌해 있는가. but, 청크 구분용으로도 쓰임
	bool was_colliding = false; // 이전 프레임에 충돌해 있었는가
};

class MAZE {
private:
public:
    std::vector<std::vector<MAZE_mem>> maze;
    std::vector<MazeBlockInstance> mazeBlocks;
    void update_world_obb();

    void printMaze();
    void generateMaze(int cx, int cy);
    void generatetype();
    void initmaze();
    void setMaze();
    void title_maze();
};
