#include "maze.h"

std::random_device rd;
std::mt19937 mt(rd());

int maze_y = 5;
int maze_x = 7;

enum PATH_WALL {
    PATH = 0,
    WALL = 1
};

struct MAZE {
	int path_wall = WALL; // 0: path, 1: wall
	int type = 15;      // 0µ¿ 1¼­ 2³² 3ºÏ  4¤Ñ 5¤Ó  6¦£ 7¦¤ 8¦¦ 9¦¥  10¤¿ 11¤Ã 12¤Ì 13¤Ç  14+ 15x
};

std::vector<std::vector<MAZE>> maze;

void printMaze() {
    for (int y = 0; y < maze_y; ++y) {
        for (int x = 0; x < maze_x; ++x) {
            if (maze[y][x].path_wall == WALL) {
                std::cout << "¡á";
            }
            else {
                std::cout << "¡à";
            }
        }

        std::cout << "\t";
        for (int x = 0; x < maze_x; ++x) {
            std::cout << maze[y][x].type << " ";
        }
        std::cout << std::endl;
    }
}

void generateMaze(int cx, int cy) {
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

void generatetype() {
    for (int y = 1; y < maze_y-1; ++y) {
        for (int x = 1; x < maze_x-1; ++x) {
			bool right, left, up, down; // true: path, false: wall

			maze[y - 1][x].path_wall == PATH ? up = true : up = false;
			maze[y + 1][x].path_wall == PATH ? down = true : down = false;
			maze[y][x - 1].path_wall == PATH ? left = true : left = false;
			maze[y][x + 1].path_wall == PATH ? right = true : right = false;

			if (up && down && left && right) maze[y][x].type = 14; // +
			else if (up && !down && left && right) maze[y][x].type = 13; // ¤Ç
			else if (!up && down && left && right) maze[y][x].type = 12; // ¤Ì
			else if (up && down && left && !right) maze[y][x].type = 11; // ¤Ã
			else if (up && down && !left && right) maze[y][x].type = 10; // ¤¿
			else if (up && !down && left && !right) maze[y][x].type = 9; // ¦¥
			else if (up && !down && !left && right) maze[y][x].type = 8; // ¦¦
			else if (!up && down && left && !right) maze[y][x].type = 7; // ¦¤
			else if (!up && down && !left && right) maze[y][x].type = 6; // ¦£
			else if (up && down && !left && !right) maze[y][x].type = 5; // ¤Ó
			else if (!up && !down && left && right) maze[y][x].type = 4; // ¤Ñ
            else if (up && !down && !left && !right) maze[y][x].type = 3; // ºÏ
            else if (!up && down && !left && !right) maze[y][x].type = 2; // ³²
            else if (!up && !down && left && !right) maze[y][x].type = 1; // ¼­
			else if (!up && !down && !left && right) maze[y][x].type = 0; // µ¿
			else maze[y][x].type = 15; // x
        }
    }
}



void setMaze() {
    std::vector<std::vector<MAZE>> a(maze_y, std::vector<MAZE>(maze_x));
    maze = a;

    generateMaze(1, 1);
    generatetype();
    printMaze();
}


void init_maze() {
    int cube_idx = 0;

    //for (int i = 0; i < maze_y; i++) {
    //    for (int j = 0; j < maze_x; j++) {
    //
    //        float x_pos = BOX_SIZE / 2 + (BOX_SIZE * j) - ((BOX_SIZE * (float)maze_x) / 2);
    //        float z_pos = BOX_SIZE / 2 + (BOX_SIZE * i) - ((BOX_SIZE * (float)maze_y) / 2);
    //        shapes[cube_idx].reset = glm::vec3(x_pos, 0.0f, z_pos);
    //        if (cube_idx == 1) {
    //            p_x = x_pos;
    //            p_y = 0.02f;
    //            p_z = z_pos;
    //        }
    //
    //        if (maze[i][j] == WALL) shapes[cube_idx].draw = true;
    //        else if (maze[i][j] == PATH) shapes[cube_idx].draw = false;
    //        cube_idx++;
    //    }
    //}

}