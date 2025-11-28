#pragma once
#include "headers.h"
#include "static_model.h"

struct targetInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
    glm::vec3 reset;

    OBB target_obb;
    bool is_break = false;  // 돌멩이에 맞아서 부서졌는가
	bool is_in_chunk = false; // 청크 내에 있는가

	float rotation_angle = 0.0f;
	int timer = 0;

    glm::vec3 origin_pos;
    glm::vec3 goal_pos;
    float move_t = 0.0f;
};

class TARGET
{
private:
public:
	vector<targetInstance> targetBlocks;
    void init(StaticModel* target_model,int target_cnt);

    void update();

    void update_world_obb();
    
};

