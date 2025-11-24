#pragma once
#include "headers.h"
#include "texture.h"
#include "maze.h"

void update_world_obb(MazeBlockInstance& shape);

bool is_separated(const OBB& a, const OBB& b, const glm::vec3& axis);

bool road_check_collision(const MazeBlockInstance& shapeA, const MazeBlockInstance& shapeB);