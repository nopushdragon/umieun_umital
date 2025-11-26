#pragma once
#include "headers.h"
#include "texture.h"

bool is_separated(const OBB& a, const OBB& b, const glm::vec3& axis);

bool check_collision(const OBB& obbA, const OBB& obbB);	// 비교할 OBB는 월드 OBB이어야 함

void drawDebugOBB(GLuint shaderID, const OBB& obb, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);