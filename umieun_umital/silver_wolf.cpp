#include "silver_wolf.h"

silver_wolf::silver_wolf()
{
   /* silverWolfModel[0] = new NewModel("silver_wolf/Idle.fbx");
    silverWolfModel[0]->state = "idle";
    silverWolfModel[1] = new NewModel("silver_wolf/Walk.fbx");
    silverWolfModel[1]->state = "walk";
    silverWolfModel[2] = new NewModel("silver_wolf/Stop Walking.fbx");
    silverWolfModel[2]->state = "stop_walking";
    silverWolfModel[3] = new NewModel("silver_wolf/Running.fbx");
    silverWolfModel[3]->state = "running";
    silverWolfModel[4] = new NewModel("silver_wolf/Run To Stop.fbx");
    silverWolfModel[4]->state = "run_to_stop";
    silverWolfModel[5] = new NewModel("silver_wolf/Throw.fbx");
    silverWolfModel[5]->state = "throw";
    silverWolfModel[6] = new NewModel("silver_wolf/Stand To Roll.fbx");
    silverWolfModel[6]->state = "stand_to_roll";*/

}

void silver_wolf::Init() {
	pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	scale = glm::vec3(0.01f);
	angle = 0.0f;
	set_obb();
}

void silver_wolf::Update(float deltatime) {


	
	State(state,deltatime);
	StateChange();
}
void silver_wolf::StateChange() {
	bool isMoving = w_press || a_press || s_press || d_press;
	if (isMoving) {
		if (state != "walk") {
			state = "walk";
		}
	}
	else {
		if (state != "idle") {
			state = "idle";
		}
	}
	
}

void silver_wolf::State(string state,float deltatime) {
	if (state == "idle") {
		// idle 상태일 때의 동작
	}
	else if (state == "walk") {
		//예외
		int move_count = w_press+ s_press+ a_press+ d_press;
		if (move_count > 2)return;


		//여긴 이동만
		if (w_press) pos.z += walkSpeed * deltatime;
		if (s_press) pos.z -= walkSpeed * deltatime;
		if (a_press) pos.x += walkSpeed * deltatime;
		if (d_press) pos.x -= walkSpeed * deltatime;

		//여기는 회전까지
		if (w_press) angle = 0.0f;
		if (s_press) angle = 180.0f;
		if (a_press) angle = 90.0f;
		if (d_press) angle = -90.0f;
		if (w_press && a_press) angle = 45.0f;
		if (w_press && d_press) angle = -45.0f;
		if (s_press && a_press) angle = 135.0f;
		if (s_press && d_press) angle = -135.0f;


		
		
	}
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



void silver_wolf::Draw(GLuint shaderID, float currentTime=0.0f) {

	for (int i = 0; i < silver_wolf_fbx_size; i++) {
		if (silverWolfModel[i]->state == state) {
            silverWolfModel[i]->pos = pos;
			silverWolfModel[i]->scale = scale;
			silverWolfModel[i]->angle = angle;
			silverWolfModel[i]->Draw(shaderID, currentTime);
		}
	}

}

silver_wolf::~silver_wolf()
{
	for (int i = 0; i < silver_wolf_fbx_size; i++)
	{
		delete& silverWolfModel[i];
	}
}


void silver_wolf::set_obb() {  //민용
	silverwolf_local_obb.center = glm::vec3(0.0f, 70.0f, 0.0f);;
	silverwolf_local_obb.half_length = glm::vec3(10.0f, 70.0f, 10.0f);
	silverwolf_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	silverwolf_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	silverwolf_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}