#include "trainer.h"
#include "trainer_state_machine.h"
uniform_int_distribution<int> random_model_num(0, 3);
uniform_int_distribution<int> random_direction(1, 7);

Trainer::Trainer(float x_pos, float z_pos)
{
	currentState = new State_Trainer_Walk();
	currentState->Enter(this);
	radom_index = random_model_num(mt);
	pos = glm::vec3(x_pos, 0.0f, z_pos);
}

void Trainer::Init()
{
	if (radom_index == 0) {
		trainerModel[0] = mei_model[0];
		trainerModel[1] = mei_model[1];
		trainerModel[2] = mei_model[2];

		scale = glm::vec3(0.005f);
	}
	else if (radom_index == 1) {
		trainerModel[0] = serena_model[0];
		trainerModel[1] = serena_model[1];
		trainerModel[2] = serena_model[2];

		scale = glm::vec3(0.2f);
	}
	else if (radom_index == 2) {
		trainerModel[0] = archie_model[0];
		trainerModel[1] = archie_model[1];
		trainerModel[2] = archie_model[2];

		scale = glm::vec3(0.005f);
	}
	else if (radom_index == 3) {
		trainerModel[0] = press_model[0];
		trainerModel[1] = press_model[1];
		trainerModel[2] = press_model[2];

		scale = glm::vec3(0.005f);
	}

	state = "walk";
	angle = 0.0f;

	set_obb();
	set_obb2();
}

void Trainer::Update(float deltatime, const bool& right_mouth,const glm::vec3& silverwolf)
{
	camera_right_mouth = right_mouth;
	old_pos = pos;

	local_anim_time += deltatime;

	if (currentState) currentState->Update(this, deltatime);
	if (aggravation) {
		silverWolf = silverwolf;

	}

	soundManager.UpdateChannelPosition(thisChannel, pos);



	update_world_obb();
}

void Trainer::Draw(GLuint shaderID, float deltatime, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
	for (int i = 0;i < 3;++i) {
		trainerModel[i]->pos = pos;
		trainerModel[i]->scale = scale;
		trainerModel[i]->angle = angle;
	}

	currentState->Draw(this, shaderID, local_anim_time);

}

void Trainer::DebugOBB(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
	drawDebugOBB(shaderID, trainer_world_obb, view, proj, color);

	drawDebugOBB(shaderID, trainer_world_obb2, view, proj, glm::vec3(0.0,0.0,0.0));
}
void Trainer::set_obb() {
	trainer_local_obb.center = glm::vec3(0.0f, 70.0f, 0.0f);
	trainer_local_obb.half_length = glm::vec3(30.0f, 70.0f, 30.0f);
	trainer_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	trainer_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	trainer_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}

void Trainer::set_obb2() {
	trainer_local_obb2.center = glm::vec3(0.0f, 70.0f, 0.0f);
	trainer_local_obb2.half_length = glm::vec3(600.0f, 70.0f, 600.0f);
	trainer_local_obb2.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	trainer_local_obb2.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	trainer_local_obb2.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}

void Trainer::update_world_obb() {
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, pos);
	//modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.005f));
	glm::mat3 rotation_scale_mat = modelMat;
	const OBB& new_trainer_local_obb = trainer_local_obb;
	const OBB& new_trainer_local_obb2 = trainer_local_obb2;
	glm::vec4 trainer_local_center_h = glm::vec4(new_trainer_local_obb.center, 1.0f);
	glm::vec4 trainer_local_center_h2 = glm::vec4(new_trainer_local_obb2.center, 1.0f);
	glm::vec3 world_center = glm::vec3(modelMat * trainer_local_center_h);
	glm::vec3 world_center2 = glm::vec3(modelMat * trainer_local_center_h2);
	trainer_world_obb.center = world_center;
	trainer_world_obb2.center = world_center2;
	for (int i = 0; i < 3; i++) {
		glm::vec3 world_axis = rotation_scale_mat * new_trainer_local_obb.u[i];
		glm::vec3 world_axis2 = rotation_scale_mat * new_trainer_local_obb2.u[i];
		if (glm::length(world_axis) > 1e-6) {
			trainer_world_obb.u[i] = glm::normalize(world_axis);
			trainer_world_obb2.u[i] = glm::normalize(world_axis2);
		}
		else {
			trainer_world_obb.u[i] = new_trainer_local_obb.u[i];
			trainer_world_obb2.u[i] = new_trainer_local_obb2.u[i];
		}
	}
	glm::vec3 road_scale_factors = glm::vec3(
		glm::length(rotation_scale_mat[0]),
		glm::length(rotation_scale_mat[1]),
		glm::length(rotation_scale_mat[2])
	);
	trainer_world_obb.half_length = trainer_local_obb.half_length * road_scale_factors;
	trainer_world_obb2.half_length = trainer_local_obb2.half_length * road_scale_factors;
}

void Trainer::ChangeState(TrainerState* newState) {
	if (currentState) {
		currentState->Exit(this); // 이전 상태 정리 (Exit)
		delete currentState;      // 메모리 해제
	}
	currentState = newState;      // 상태 교체
	currentState->Enter(this);    // 새 상태 진입 (Enter)

}


Trainer::~Trainer()
{
	if (currentState) {
		currentState->Exit(this);
		delete currentState;
	}
	thisChannel = nullptr;
}



void State_Trainer_Walk::Enter(Trainer* trainer) {
	if (trainer->trainerModel[0]) {
		trainer->trainerModel[0]->ResetAnimation();
		trainer->time = 0.0f;
		trainer->local_anim_time = 0.0f;
	}

}

void State_Trainer_Walk::Update(Trainer* trainer, float deltatime) {
	glm::vec3 v(0.0f);
	v.z += trainer->walk_speed * deltatime;
	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(trainer->angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 1.0f));
	trainer->pos += v;
	trainer->time += deltatime;

	if (trainer->time >= 5.0f) {
		int random_dir = random_direction(mt);
		trainer->angle = random_dir * 45.0f;
		trainer->time = 0.0f;
	}

	if (trainer->aggravation) {
		trainer->ChangeState(new State_Trainer_Tracking());
	}
	if (trainer->die_change) {
		trainer->ChangeState(new State_Trainer_Die());
	}
}

void State_Trainer_Walk::Draw(Trainer* trainer, GLuint shaderID, float deltatime) {
	trainer->trainerModel[0]->Draw(shaderID, deltatime);
}

void State_Trainer_Walk::Exit(Trainer* trainer) {
}

void State_Trainer_Tracking::Enter(Trainer* trainer) {
	if (trainer->trainerModel[1]) {
		trainer->trainerModel[1]->ResetAnimation();
		trainer->local_anim_time = 0.0f;
	}
}
void State_Trainer_Tracking::Update(Trainer* trainer, float deltatime) {
	glm::vec3 direction = trainer->silverWolf - trainer->pos;
	direction.y = 0.0f; // 수평 방향으로만 회전
	direction = glm::normalize(direction);
	float target_angle = glm::degrees(atan2(-direction.x, -direction.z));
	trainer->angle = target_angle+180.0f;
	glm::vec3 v(0.0f);
	v.z += trainer->run_speed * deltatime;
	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(-trainer->angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 1.0f));
	trainer->pos.x -= v.x;
	trainer->pos.z += v.z;
	if (!trainer->aggravation) {
		trainer->ChangeState(new State_Trainer_Walk());
	}
	if (trainer->die_change) {
		trainer->ChangeState(new State_Trainer_Die());
	}
}
void State_Trainer_Tracking::Draw(Trainer* trainer, GLuint shaderID, float deltatime) {
	trainer->trainerModel[1]->Draw(shaderID, deltatime);
}

void State_Trainer_Tracking::Exit(Trainer* trainer) {
}
void State_Trainer_Die::Enter(Trainer* trainer) {
	if (trainer->trainerModel[2]) {
		trainer->trainerModel[2]->ResetAnimation();
		trainer->local_anim_time = 0.0f;
	}
}
void State_Trainer_Die::Update(Trainer* trainer, float deltatime) {
	float totalDuration = trainer->trainerModel[2]->GetDuration();
	if (trainer->local_anim_time >= totalDuration)
		trainer->die = true;
}
void State_Trainer_Die::Draw(Trainer* trainer, GLuint shaderID, float deltatime) {
	trainer->trainerModel[2]->Draw(shaderID, deltatime);
}
void State_Trainer_Die::Exit(Trainer* trainer) {
}


