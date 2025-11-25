#include "silver_wolf.h"
#include "state_machine.h"
silver_wolf::silver_wolf()
{
	currentState = new State_Idle();
	currentState->Enter(this);

}

void silver_wolf::Init() {
	pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	scale = glm::vec3(0.01f);
	angle = 0.0f;
	set_obb();
}

void silver_wolf::Update(float deltatime) {
	

	
	if (currentState) currentState->Update(this, deltatime);
}
void silver_wolf::ChangeState(WolfState* newState) {
	if (currentState) {
		currentState->Exit(this); // 이전 상태 정리 (Exit)
		delete currentState;      // 메모리 해제
	}
	currentState = newState;      // 상태 교체
	currentState->Enter(this);    // 새 상태 진입 (Enter)
	
}

//void silver_wolf::State(string state,float deltatime) {
//	if (state == "idle") {
//		// idle 상태일 때의 동작
//	}
//	else if (state == "walk") {
//		//예외
//		int move_count = w_press+ s_press+ a_press+ d_press;
//		if (move_count > 2)return;
//
//
//		//여긴 이동만
//		if (w_press) pos.z += walkSpeed * deltatime;
//		if (s_press) pos.z -= walkSpeed * deltatime;
//		if (a_press) pos.x += walkSpeed * deltatime;
//		if (d_press) pos.x -= walkSpeed * deltatime;
//
//		//여기는 회전까지
//		if (w_press) angle = 0.0f;
//		if (s_press) angle = 180.0f;
//		if (a_press) angle = 90.0f;
//		if (d_press) angle = -90.0f;
//		if (w_press && a_press) angle = 45.0f;
//		if (w_press && d_press) angle = -45.0f;
//		if (s_press && a_press) angle = 135.0f;
//		if (s_press && d_press) angle = -135.0f;
//
//
//		
//		
//	}
//}



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



void silver_wolf::Draw(GLuint shaderID, float currentTime=0.0f) {

	currentState->Draw(this, shaderID, currentTime);

}

silver_wolf::~silver_wolf()
{
	if (currentState) {
		currentState->Exit(this);
		delete currentState;
	}
}


void silver_wolf::set_obb() {  //민용
	silverwolf_local_obb.center = glm::vec3(0.0f, 70.0f, 0.0f);;
	silverwolf_local_obb.half_length = glm::vec3(10.0f, 70.0f, 10.0f);
	silverwolf_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	silverwolf_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	silverwolf_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}

//그냥 아이들 멈춰있는 상태 idle
void State_Idle::Enter(silver_wolf* wolf) {

}
void State_Idle::Update(silver_wolf* wolf, float detatime) {

}
void State_Idle::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[0]->Draw(shaderID,time);
}
void State_Idle::Exit(silver_wolf* wolf) {

}

//걷기 워크 wolk
void State_Walk::Enter(silver_wolf* wolf) {

}
void State_Walk::Update(silver_wolf* wolf, float detatime) {

}
void State_Walk::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[1]->Draw(shaderID, time);
}
void State_Walk::Exit(silver_wolf* wolf) {

}

//걷기 멈춤 스탑 워크 stop wolk
void State_Stop_Walk::Enter(silver_wolf* wolf) {

}
void State_Stop_Walk::Update(silver_wolf* wolf, float detatime) {

}
void State_Stop_Walk::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[2]->Draw(shaderID, time);
}
void State_Stop_Walk::Exit(silver_wolf* wolf) {

}


//달리기 런 run
void State_Run::Enter(silver_wolf* wolf) {

}
void State_Run::Update(silver_wolf* wolf, float detatime) {

}
void State_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[3]->Draw(shaderID, time);
}
void State_Run::Exit(silver_wolf* wolf) {

}


//달리기 멈춤 런 스탐 run stop
void State_Stop_Run::Enter(silver_wolf* wolf) {

}
void State_Stop_Run::Update(silver_wolf* wolf, float detatime) {

}
void State_Stop_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[4]->Draw(shaderID, time);
}
void State_Stop_Run::Exit(silver_wolf* wolf) {

}

//던지기 쓰로우 throw
void State_Throw::Enter(silver_wolf* wolf) {
}
void State_Throw::Update(silver_wolf* wolf, float detatime) {
}
void State_Throw::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[5]->Draw(shaderID, time);
}
void State_Throw::Exit(silver_wolf* wolf) {
}

//구르기 롤 roll
void State_Roll::Enter(silver_wolf* wolf) {
}
void State_Roll::Update(silver_wolf* wolf, float detatime) {
}
void State_Roll::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[6]->Draw(shaderID, time);
}
void State_Roll::Exit(silver_wolf* wolf) {
}

//점프 뛰기 jump
void State_Jump::Enter(silver_wolf* wolf) {
}
void State_Jump::Update(silver_wolf* wolf, float detatime) {
}
void State_Jump::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[7]->Draw(shaderID, time);
}
void State_Jump::Exit(silver_wolf* wolf) {
}

//달리면서 점프 Run Jump
void State_Jump_Run::Enter(silver_wolf* wolf) {
}
void State_Jump_Run::Update(silver_wolf* wolf, float detatime) {
}
void State_Jump_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[8]->Draw(shaderID, time);
}
void State_Jump_Run::Exit(silver_wolf* wolf) {
}




