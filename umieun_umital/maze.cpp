#include "maze.h"

std::random_device rd;
std::mt19937 mt(rd());

void MAZE::printMaze() {
    for (int y = 0; y < maze_y; ++y) {
        for (int x = 0; x < maze_x; ++x) {
            if (maze[y][x].path_wall == WALL) {
                std::cout << "■";
            }
            else {
                std::cout << "□";
            }
        }

        std::cout << "\t";
        for (int x = 0; x < maze_x; ++x) {
            std::cout << maze[y][x].type << " ";
        }
        std::cout << std::endl;
    }
}

void MAZE::generateMaze(int cx, int cy) {
    maze[cy][cx].path_wall = PATH;

    std::vector<std::pair<int, int>> directions = {
        {0, -2},
        {0, 2},
        {-2, 0},
        {2, 0}
    };

    std::shuffle(directions.begin(), directions.end(), mt);

    for (const auto& dir : directions) {
        int nx = cx + dir.first;
        int ny = cy + dir.second;

        if (nx > 0 && nx < maze_x - 1 && ny > 0 && ny < maze_y - 1 && maze[ny][nx].path_wall == WALL) {

            int wall_x = cx + dir.first / 2;
            int wall_y = cy + dir.second / 2;
            maze[wall_y][wall_x].path_wall = PATH;

            generateMaze(nx, ny);
        }
    }
}

void MAZE::generatetype() {
    for (int y = 1; y < maze_y-1; ++y) {
        for (int x = 1; x < maze_x-1; ++x) {

			if (maze[y][x].path_wall == WALL) continue;

			bool right, left, up, down; // true: path, false: wall

			maze[y - 1][x].path_wall == PATH ? up = true : up = false;
			maze[y + 1][x].path_wall == PATH ? down = true : down = false;
			maze[y][x - 1].path_wall == PATH ? left = true : left = false;
			maze[y][x + 1].path_wall == PATH ? right = true : right = false;

			if (up && down && left && right) maze[y][x].type = 14; // +
			else if (up && !down && left && right) maze[y][x].type = 13; // ㅗ
			else if (!up && down && left && right) maze[y][x].type = 12; // ㅜ
			else if (up && down && left && !right) maze[y][x].type = 11; // ㅓ
			else if (up && down && !left && right) maze[y][x].type = 10; // ㅏ
			else if (up && !down && left && !right) maze[y][x].type = 9; // ┘
			else if (up && !down && !left && right) maze[y][x].type = 8; // └
			else if (!up && down && left && !right) maze[y][x].type = 7; // ┐
			else if (!up && down && !left && right) maze[y][x].type = 6; // ┌
			else if (up && down && !left && !right) maze[y][x].type = 5; // ㅣ
			else if (!up && !down && left && right) maze[y][x].type = 4; // ㅡ
            else if (up && !down && !left && !right) maze[y][x].type = 3; // 북
            else if (!up && down && !left && !right) maze[y][x].type = 2; // 남
            else if (!up && !down && left && !right) maze[y][x].type = 1; // 서
			else if (!up && !down && !left && right) maze[y][x].type = 0; // 동
			else maze[y][x].type = 15; // x
        }
    }
}

void MAZE::initmaze(std::vector<StaticModel*>* roads) {
    // road 인스턴스를 씬 중앙에 배치
    for (int i = 0; i < maze_y; i++) {
        for (int j = 0; j < maze_x; j++) {
            MazeBlockInstance roadInstance;
			roadInstance.modelPtr = roads->at(maze[i][j].type);
            float x_pos = ROAD_SIZE / 2 + (ROAD_SIZE * j) - ((ROAD_SIZE * (float)maze_x) / 2);
            float z_pos = ROAD_SIZE / 2 + (ROAD_SIZE * i) - ((ROAD_SIZE * (float)maze_y) / 2);
            roadInstance.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_pos, 0.0f, z_pos));
            roadInstance.reset = glm::vec3(x_pos, 0.0f, z_pos);
            mazeBlocks.push_back(roadInstance);
        }
	}
    start_x_pos = ROAD_SIZE / 2 + (ROAD_SIZE)-((ROAD_SIZE * (float)maze_x) / 2);
    start_z_pos = ROAD_SIZE / 2 + (ROAD_SIZE)-((ROAD_SIZE * (float)maze_y) / 2);
    update_world_obb();
}

void MAZE::setMaze() {
    std::vector<std::vector<MAZE_mem>> a(maze_y, std::vector<MAZE_mem>(maze_x));
    maze = a;

    generateMaze(1, 1);
    generatetype();
    printMaze();
}

void MAZE::update_world_obb() {
    for (auto& block : mazeBlocks) {
        glm::mat3 rotation_scale_mat = glm::mat3(block.modelMatrix);

        // Local OBB 데이터 (StaticModel 내 공유되는 데이터)
        const OBB& road_local_obb = block.modelPtr->road_local_obb;

        //=========================================
        // road_local_obb를 road_world_obb로 변환 (이제 block 인스턴스의 멤버에 씁니다)
        glm::vec4 road_local_center_h = glm::vec4(road_local_obb.center, 1.0f);
        glm::vec3 world_center = glm::vec3(block.modelMatrix * road_local_center_h);

        block.road_world_obb.center = world_center;

        for (int i = 0; i < 3; i++) {
            glm::vec3 world_axis = rotation_scale_mat * road_local_obb.u[i];

            if (glm::length(world_axis) > 1e-6) {
                block.road_world_obb.u[i] = glm::normalize(world_axis);
            }
            else {
                block.road_world_obb.u[i] = road_local_obb.u[i];
            }
        }

        glm::vec3 road_scale_factors = glm::vec3(
            glm::length(rotation_scale_mat[0]),
            glm::length(rotation_scale_mat[1]),
            glm::length(rotation_scale_mat[2])
        );

        block.road_world_obb.half_length = road_local_obb.half_length * road_scale_factors;
        //=========================================

        // obstacle_world_obb 업데이트
        block.obstacle_world_obb.resize(block.modelPtr->obstacle_local_obb.size()); // World OBB 벡터 크기 조정

        for (size_t i = 0; i < block.modelPtr->obstacle_local_obb.size(); i++) {
            const OBB& obstacle_local_obb = block.modelPtr->obstacle_local_obb[i];

            glm::vec4 obstacle_local_center_h = glm::vec4(obstacle_local_obb.center, 1.0f);
            glm::vec3 obs_world_center = glm::vec3(block.modelMatrix * obstacle_local_center_h);

            block.obstacle_world_obb[i].center = obs_world_center;

            for (int j = 0; j < 3; j++) {
                glm::vec3 world_axis = rotation_scale_mat * obstacle_local_obb.u[j];

                if (glm::length(world_axis) > 1e-6) {
                    block.obstacle_world_obb[i].u[j] = glm::normalize(world_axis);
                }
                else {
                    block.obstacle_world_obb[i].u[j] = obstacle_local_obb.u[j];
                }
            }

            glm::vec3 obstacle_scale_factors = glm::vec3(
                glm::length(rotation_scale_mat[0]),
                glm::length(rotation_scale_mat[1]),
                glm::length(rotation_scale_mat[2])
            );

            block.obstacle_world_obb[i].half_length = obstacle_local_obb.half_length * obstacle_scale_factors;
        }
    }
}