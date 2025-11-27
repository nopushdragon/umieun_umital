#pragma once
#include "headers.h"
#include "static_model.h"

struct targetInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
    glm::vec3 reset;

    OBB target_obb;
    bool is_colliding = false;
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

