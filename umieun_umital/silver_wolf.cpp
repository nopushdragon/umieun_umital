#include "silver_wolf.h"
#include "state_machine.h"
silver_wolf::silver_wolf()
{
	currentState = new State_Idle();
	currentState->Enter(this);

}

void silver_wolf::Init() {
	pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	scale = glm::vec3(0.005f);
	angle = 0.0f;
	set_obb();
}

void silver_wolf::Update(float deltatime) {
	

	old_pos = pos;
	if (currentState) currentState->Update(this, deltatime);
	update_world_obb();

}
void silver_wolf::ChangeState(WolfState* newState) {
	if (currentState) {
		currentState->Exit(this); // 이전 상태 정리 (Exit)
		delete currentState;      // 메모리 해제
	}
	currentState = newState;      // 상태 교체
	currentState->Enter(this);    // 새 상태 진입 (Enter)
	
}



void silver_wolf::Keyboard(unsigned char key, int x, int y) {

	switch (key)
	{
	case 'w':
	case 'W':
		w_press = true;
		break;
	case 'a':
	case 'A':
		a_press = true;
		break;
	case 's':
	case 'S':
		s_press = true;
		break;
	case 'd':
	case 'D':
		d_press = true;
		break;
	}

}

void silver_wolf::Keyupboard(unsigned char key, int x, int y) {

	switch (key)
	{
	case 'w':
	case 'W':
		w_press = false;
		break;
	case 'a':
	case 'A':
		a_press = false;
		break;
	case 's':
	case 'S':
		s_press = false;
		break;
	case 'd':
	case 'D':
		d_press = false;
		break;
	}

}

void silver_wolf::SpecialKeyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_SHIFT_L:
		shift_press = true;
		break;
	}
}

void silver_wolf::SpecialUpKeyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_SHIFT_L:
		shift_press = false;
		break;
	}
}



void silver_wolf::Draw(GLuint shaderID, float currentTime=0.0f) {
	for (int i = 0;i < 9;++i) {
		silverWolfModel[i]->pos =	pos;
		silverWolfModel[i]->scale = scale;
		silverWolfModel[i]->angle = angle;
	}
	currentState->Draw(this, shaderID, currentTime);

}

silver_wolf::~silver_wolf()
{
	if (currentState) {
		currentState->Exit(this);
		delete currentState;
	}
}

void silver_wolf::ResetInputs() {
	w_press = false;
	a_press = false;
	s_press = false;
	d_press = false;
	shift_press = false;
	//space_press = false;

}

void silver_wolf::set_obb() {  //민용
	silverwolf_local_obb.center = glm::vec3(0.0f, 70.0f, 0.0f);;
	silverwolf_local_obb.half_length = glm::vec3(10.0f, 70.0f, 10.0f);
	silverwolf_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	silverwolf_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	silverwolf_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}

void silver_wolf::update_world_obb() {
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat,  pos);
	modelMat = glm::rotate(modelMat, glm::radians( angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMat = glm::scale(modelMat,  scale);

	glm::mat3 rotation_scale_mat = modelMat;

	const OBB& new_silverwolf_local_obb =  silverwolf_local_obb;

	glm::vec4 silverwolf_local_center_h = glm::vec4(new_silverwolf_local_obb.center, 1.0f);
	glm::vec3 world_center = glm::vec3(modelMat * silverwolf_local_center_h);

	 silverwolf_world_obb.center = world_center;

	for (int i = 0; i < 3; i++) {
		glm::vec3 world_axis = rotation_scale_mat * new_silverwolf_local_obb.u[i];

		if (glm::length(world_axis) > 1e-6) {
			 silverwolf_world_obb.u[i] = glm::normalize(world_axis);
		}
		else {
			 silverwolf_world_obb.u[i] = new_silverwolf_local_obb.u[i];
		}
	}

	glm::vec3 road_scale_factors = glm::vec3(
		glm::length(rotation_scale_mat[0]),
		glm::length(rotation_scale_mat[1]),
		glm::length(rotation_scale_mat[2])
	);

	 silverwolf_world_obb.half_length = silverwolf_local_obb.half_length * road_scale_factors;
}

//그냥 아이들 멈춰있는 상태 idle
void State_Idle::Enter(silver_wolf* wolf) {

}
void State_Idle::Update(silver_wolf* wolf, float detatime) {
	if((wolf->w_press|| wolf->a_press|| wolf->s_press|| wolf->d_press)&&wolf->shift_press)
	{
		wolf->ChangeState(new State_Run());
	}
	else if ((wolf->w_press || wolf->a_press || wolf->s_press || wolf->d_press))
	{
		wolf->ChangeState(new State_Walk());
	}

}
void State_Idle::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[0]->Draw(shaderID,time);
}
void State_Idle::Exit(silver_wolf* wolf) {

}

//걷기 워크 wolk
void State_Walk::Enter(silver_wolf* wolf) {

}
void State_Walk::Update(silver_wolf* wolf, float detlatime) {

	if (wolf->w_press) wolf->pos.z += wolf->walkSpeed * detlatime;
	if (wolf->s_press) wolf->pos.z -= wolf->walkSpeed * detlatime;
	if (wolf->a_press) wolf->pos.x += wolf->walkSpeed * detlatime;
	if (wolf->d_press) wolf->pos.x -= wolf->walkSpeed * detlatime;

	//여기는 회전까지
	if (wolf->w_press) wolf->angle = 0.0f;
	if (wolf->s_press) wolf->angle = 180.0f;
	if (wolf->a_press) wolf->angle = 90.0f;
	if (wolf->d_press) wolf->angle = -90.0f;
	if (wolf->w_press && wolf->a_press) wolf->angle = 45.0f;
	if (wolf->w_press && wolf->d_press) wolf->angle = -45.0f;
	if (wolf->s_press && wolf->a_press) wolf->angle = 135.0f;
	if (wolf->s_press && wolf->d_press) wolf->angle = -135.0f;

	if (!wolf->w_press && !wolf->a_press && !wolf->s_press && !wolf->d_press)
	{
		wolf->ChangeState(new State_Idle());
	}
	else if (wolf->shift_press)
	{
		wolf->ChangeState(new State_Run());
	}
}
void State_Walk::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	int press_num = wolf->w_press + wolf->a_press + wolf->s_press + wolf->d_press;
	if(press_num>2||(wolf->w_press&& wolf->s_press)|| (wolf->a_press && wolf->d_press))
		wolf->silverWolfModel[0]->Draw(shaderID, time);
	else
		wolf->silverWolfModel[1]->Draw(shaderID, time);
}
void State_Walk::Exit(silver_wolf* wolf) {

}

//걷기 멈춤 스탑 워크 stop wolk
void State_Stop_Walk::Enter(silver_wolf* wolf) {

}
void State_Stop_Walk::Update(silver_wolf* wolf, float detlatime) {

}
void State_Stop_Walk::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[2]->Draw(shaderID, time);
}
void State_Stop_Walk::Exit(silver_wolf* wolf) {

}


//달리기 런 run
void State_Run::Enter(silver_wolf* wolf) {

}
void State_Run::Update(silver_wolf* wolf, float detlatime) {
	if (wolf->w_press) wolf->pos.z += wolf->runSpeed * detlatime;
	if (wolf->s_press) wolf->pos.z -= wolf->runSpeed * detlatime;
	if (wolf->a_press) wolf->pos.x += wolf->runSpeed * detlatime;
	if (wolf->d_press) wolf->pos.x -= wolf->runSpeed * detlatime;

	//여기는 회전까지
	if (wolf->w_press) wolf->angle = 0.0f;
	if (wolf->s_press) wolf->angle = 180.0f;
	if (wolf->a_press) wolf->angle = 90.0f;
	if (wolf->d_press) wolf->angle = -90.0f;
	if (wolf->w_press && wolf->a_press) wolf->angle = 45.0f;
	if (wolf->w_press && wolf->d_press) wolf->angle = -45.0f;
	if (wolf->s_press && wolf->a_press) wolf->angle = 135.0f;
	if (wolf->s_press && wolf->d_press) wolf->angle = -135.0f;

	if (!wolf->w_press && !wolf->a_press && !wolf->s_press && !wolf->d_press)
	{
		wolf->ChangeState(new State_Idle());
	}
	else if (!wolf->shift_press)
	{
		wolf->ChangeState(new State_Walk());
	}
}
void State_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[3]->Draw(shaderID, time);
}
void State_Run::Exit(silver_wolf* wolf) {

}


//달리기 멈춤 런 스탐 run stop
void State_Stop_Run::Enter(silver_wolf* wolf) {

}
void State_Stop_Run::Update(silver_wolf* wolf, float detlatime) {

}
void State_Stop_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[4]->Draw(shaderID, time);
}
void State_Stop_Run::Exit(silver_wolf* wolf) {

}

//던지기 쓰로우 throw
void State_Throw::Enter(silver_wolf* wolf) {
}
void State_Throw::Update(silver_wolf* wolf, float detlatime) {
}
void State_Throw::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[5]->Draw(shaderID, time);
}
void State_Throw::Exit(silver_wolf* wolf) {
}

//구르기 롤 roll
void State_Roll::Enter(silver_wolf* wolf) {
}
void State_Roll::Update(silver_wolf* wolf, float detlatime) {
}
void State_Roll::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[6]->Draw(shaderID, time);
}
void State_Roll::Exit(silver_wolf* wolf) {
}

//점프 뛰기 jump
void State_Jump::Enter(silver_wolf* wolf) {
}
void State_Jump::Update(silver_wolf* wolf, float detlatime) {
}
void State_Jump::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[7]->Draw(shaderID, time);
}
void State_Jump::Exit(silver_wolf* wolf) {
}

//달리면서 점프 Run Jump
void State_Jump_Run::Enter(silver_wolf* wolf) {
}
void State_Jump_Run::Update(silver_wolf* wolf, float detlatime) {
}
void State_Jump_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[8]->Draw(shaderID, time);
}
void State_Jump_Run::Exit(silver_wolf* wolf) {
}




