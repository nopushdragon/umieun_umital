#pragma once
#include "headers.h"
#include "texture.h"
#include "maze.h"
#include "silver_wolf.h"

void maze_update_world_obb(MazeBlockInstance& shape);
void silver_wolf_update_world_obb(silver_wolf& silverwolf);

bool is_separated(const OBB& a, const OBB& b, const glm::vec3& axis);

bool check_collision(const OBB& obbA, const OBB& obbB);	// 비교할 OBB는 월드 OBB이어야 함