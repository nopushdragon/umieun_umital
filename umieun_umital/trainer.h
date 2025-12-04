#pragma once
#include "resource.h"
#include "collision.h"



class TrainerState;

class Trainer
{
private:

public:
	TrainerState* currentState;
	float walk_speed = 1.0f;
	float run_speed = 2.0f;
	NewModel* trainerModel[3];
	int radom_index;
	string state = "walk";
	bool die_change = false;
	bool die = false;
	bool camera_right_mouth = false;
	bool aggravation = false;
	bool voice = false;
	float local_anim_time = 0.0f;
	bool is_in_chunk = false;
	Channel* thisChannel = nullptr;

	glm::vec3 pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	glm::vec3 old_pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	glm::vec3 scale = glm::vec3(0.005f);
	float angle = 0.0f;
	glm::mat4 modelMat = glm::mat4(1.0f);

	OBB trainer_local_obb;
	OBB trainer_local_obb2;
	OBB trainer_world_obb;
	OBB trainer_world_obb2;
	glm::vec3 silverWolf;

	float time = 0.0f;
	
	bool is_in_chunk = false;


	void set_obb();

	void update_world_obb();

	void set_obb2();


	Trainer(float x_pos, float z_pos);

	void Init();

	void Update(float deltatime, const bool& right_mouth,const glm::vec3& silverwolf);

	void ChangeState(TrainerState* newState);

	void Draw(GLuint shaderID, float deltatime, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);



	void DebugOBB(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);


	~Trainer();

};

