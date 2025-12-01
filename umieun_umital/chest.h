#pragma once
#include "headers.h"
#include "static_model.h"
#include "resource.h"

struct chestInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
    glm::vec3 reset;

    OBB chest_obb;
    OBB chest_nearby_obb;
    bool is_nearby = false; //플레이어가 상자에 닿았는가
    bool is_in_chunk = false; // 청크 내에 있는가

    float rotation_angle = 0.0f;
    int timer = 0;

    glm::vec3 origin_pos;
    glm::vec3 goal_pos;
    float move_t = 0.0f;
};

class CHEST
{
private:
public:
    vector<chestInstance> chestBlocks;
    void init(glm::vec3 reset_matrix);

    void update();

    void update_world_obb();

	void open_chest();

};



