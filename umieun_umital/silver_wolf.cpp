#include "silver_wolf.h"
#include "state_machine.h"

silver_wolf::silver_wolf()
{
	currentState = new State_Idle();
	currentState->Enter(this);

}

void silver_wolf::Init() {
	silverWolfModel[0] = new NewModel("silver_wolf/Idle.fbx");
	silverWolfModel[0]->state = "idle";
	silverWolfModel[1] = new NewModel("silver_wolf/Walk.fbx");
	silverWolfModel[1]->state = "walk";
	silverWolfModel[2] = new NewModel("silver_wolf/Running.fbx");
	silverWolfModel[2]->state = "run";
	silverWolfModel[3] = new NewModel("silver_wolf/Throw.fbx");
	silverWolfModel[3]->state = "throw";
	silverWolfModel[4] = new NewModel("silver_wolf/Jump Over.fbx");
	silverWolfModel[4]->state = "roll";
	silverWolfModel[5] = new NewModel("silver_wolf/Jump.fbx");
	silverWolfModel[5]->state = "jump";
	silverWolfModel[6] = new NewModel("silver_wolf/Running Jump.fbx");
	silverWolfModel[6]->state = "jump_run";
	silverWolfModel[7] = new NewModel("silver_wolf/Run To Stop.fbx");
	silverWolfModel[7]->state = "stop_run";
	silverWolfModel[8] = new NewModel("silver_wolf/Backflip.fbx");
	silverWolfModel[8]->state = "jump_idle";



	pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	scale = glm::vec3(0.005f);
	angle = 0.0f;
	init_success = false;
	set_obb();
}

void silver_wolf::Update(float deltatime, const float& camera_x_angle, const float& camera_y_angle, const bool& right_mouth) {
	

	old_pos = pos;
	camera_angle = camera_x_angle;
	camera_right_mouth = right_mouth;

	local_anim_time += deltatime;

	if (currentState) currentState->Update(this, deltatime);



	if (camera_right_mouth) {
		glm::mat4 m = glm::rotate(glm::mat4(1.0f), glm::radians(-camera_x_angle), glm::vec3(0.0f, 1.0f, 0.0f));
		//pos = glm::vec3(m * glm::vec4(pos, 1.0f));
		angle = -camera_x_angle;
		old_camera_angle = camera_x_angle;

		Calculate(camera_y_angle);


	}

	if (!ball.empty()) {
		for (int i = ball.size() - 1;i >= 0;--i) {
			ball[i].Update(deltatime);
			if(ball[i].end_pos)
				ball.erase(ball.begin() + i);
		}
	}


	update_world_obb();
}

void silver_wolf::Calculate(const float& camera_y_angle) {

	//p1
	glm::vec3 handOffset = glm::vec3(0.0f, 2.0f, 0.0f);
	p0 = pos + handOffset;


	float angle_abs = std::abs(camera_y_angle);

	float distance = max_rock_distance - (rock_distance / max_angle_difference * angle_abs);

	glm::vec4 localDist = glm::vec4(0.0f, 0.0f, distance, 1.0f);

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-old_camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));


	//p2
	p2 = glm::vec3(rotation * localDist);
	p2.x += pos.x;
	p2.z += pos.z;
	p2.y = 0.0f; 


	glm::vec3 midPoint = (p0 + p2) * 0.5f;


	float clampedPitch = std::max(0.0f, camera_y_angle);
	float arcFactor = 0.5f;


	float addedHeight = max_rock_distance * std::tan(glm::radians(clampedPitch)) * arcFactor*5;
	p1 = midPoint;
	p1.y += addedHeight;


	p0.y -= 1.3f;

	glm::vec3 v(0.0f);v.x = 0.1f;
	glm::mat4 m = glm::rotate(glm::mat4(1.0f), glm::radians(-old_camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 1.0f));

	p0 -= v;

	//버텍스 넣어주기
	for (int i = 0;i < 500;++i) {
		float t = static_cast<float>(i) / 499.0f;
		glm::vec3 pointOnCurve = (1 - t) * (1 - t) * p0 + 2 * (1 - t) * t * p1 + t * t * p2;
		rock_path[i] = pointOnCurve;
	}

	/*cout << camera_y_angle << endl;
	cout << "p0: " << p0.x << ", " << p0.y << ", " << p0.z << endl;
	cout << "p1: " << rock_height_pos.x << ", " << rock_height_pos.y << ", " << rock_height_pos.z << endl;
	cout << "p2: " << p2.x << ", " << p2.y << ", " << p2.z << endl;*/
}



void silver_wolf::Draw(GLuint shaderID, float currentTime, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
	for (int i = 0;i < silver_wolf_fbx_size;++i) {
		silverWolfModel[i]->pos = pos;
		silverWolfModel[i]->scale = scale;
		silverWolfModel[i]->angle = angle;
	}
	
	currentState->Draw(this, shaderID, local_anim_time);

	if (camera_right_mouth) {
		Rock_path_draw(shaderID, view,proj, color);
	}

}
void silver_wolf::Ball_Draw(GLuint shaderProgramStatic, float currentTime, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {


	if (!ball.empty()) {
		for (int i = ball.size() - 1;i >= 0;--i) {
			ball[i].Draw(shaderProgramStatic);
		}
	}



}

void silver_wolf::DebugOBB(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
	drawDebugOBB(shaderID, silverwolf_world_obb, view, proj, color);


}

void silver_wolf::DebugOBB_Ball(GLuint shaderProgramStatic, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {

	if (!ball.empty()) {
		for (int i = ball.size() - 1;i >= 0;--i) {
			ball[i].DebugOBB(shaderProgramStatic, view, proj, glm::vec3(1.0f, 0.0, 1.0f));
		}
	}

}

void silver_wolf::Rock_path_draw(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color) {
	if (shaderID == 0) return;

	glUseProgram(shaderID);

	// 1. 필수 공통 유니폼 설정 (이동, 카메라 위치, 조명 위치 등)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(view[0]));

	GLint originalBUseTexture;
	glm::vec3 originalMaterialColorDefault;
	float originalAmbientStrength;
	int originalShininess;
	glm::vec3 originalMaterialSpecular;

	// A. 현재 상태 저장 (이 값들을 복원해야 함)
	glGetUniformiv(shaderID, glGetUniformLocation(shaderID, "bUseTexture"), &originalBUseTexture);
	glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "materialColorDefault"), glm::value_ptr(originalMaterialColorDefault));
	glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "ambientStrength"), &originalAmbientStrength);
	glGetUniformiv(shaderID, glGetUniformLocation(shaderID, "shininess"), &originalShininess);
	glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "materialSpecular"), glm::value_ptr(originalMaterialSpecular));

	// B. 조명 무시를 위한 값 강제 설정
	glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), 0); // 텍스처 비활성화 [cite: 2, 5]
	glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(color)); // 기본 색상으로 디버그 색상 주입 [cite: 2, 7]
	glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), 1.0f); // 앰비언트 최대화 (조명 색상 * 기본 색상) [cite: 3, 7]
	glUniform1f(glGetUniformLocation(shaderID, "fogEnabled"), false);
	glUniform1i(glGetUniformLocation(shaderID, "shininess"), 1); // pow(..., 1)이 되어 specular가 조명 강도에만 영향을 받게 하거나, 

	// 0으로 설정하여 아예 거울 반사를 없앨 수 있습니다.
	// 여기서는 1로 두어 스페큘러 항을 최소화하고, materialSpecular를 0으로 설정합니다. [cite: 3, 11]
	glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(glm::vec3(0.0f))); // 스페큘러 기여도 0으로 설정 [cite: 3, 12]



	GLuint rockVAO, rockVBO;
	glGenVertexArrays(1, &rockVAO);
	glGenBuffers(1, &rockVBO);
	// --- VAO 설정 ---
	glBindVertexArray(rockVAO);

	glBindBuffer(GL_ARRAY_BUFFER, rockVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rock_path), &rock_path, GL_STATIC_DRAW);


	// 정점 속성 포인터 설정 (기존 셰이더의 aPos 위치 0 사용)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glDisableVertexAttribArray(1); // aNormal (Location 1) 
	glDisableVertexAttribArray(2); // aTexCoords (Location 2)


	glm::mat4 model = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

	// 드로우 호출 (GL_LINES)
	glBindVertexArray(rockVAO);
	glDrawArrays(GL_LINE_STRIP, 0, 500);

	
	glBindVertexArray(0);

	// --- 리소스 해제 ---
	glDeleteBuffers(1, &rockVBO);
	glDeleteVertexArrays(1, &rockVAO);

	// 환경 복원
	glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), originalBUseTexture);
	glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(originalMaterialColorDefault));
	glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), originalAmbientStrength);
	glUniform1i(glGetUniformLocation(shaderID, "shininess"), originalShininess);
	//glUniform1f(glGetUniformLocation(shaderID, "fogEnabled"), true);
	glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(originalMaterialSpecular));

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
	case 'f':
	case 'F':
		if(current_animation_index>=0&& current_animation_index <= 2&&!camera_right_mouth)
			ChangeState(new State_Roll());
		break;
	case ' ':
		if (!camera_right_mouth) {
			if (current_animation_index == 0)
				ChangeState(new State_Jump_Idle());
			else if (current_animation_index == 1)
				ChangeState(new State_Jump());
			else if (current_animation_index == 2)
				ChangeState(new State_Jump_Run());
		}

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

void silver_wolf::Mouse(int button, int state, int x, int yh) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN&&!throw_press&& camera_right_mouth) {
		if (ball_cnt > 0) {
			ball_cnt--;
			ChangeState(new State_Throw());
			throw_press = true;
		}
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





silver_wolf::~silver_wolf()
{
	if (currentState) {
		currentState->Exit(this);
		delete currentState;
	}
}

void silver_wolf::ForceToIdle() {
	w_press = false;
	a_press = false;
	s_press = false;
	d_press = false;
	shift_press = false;

	ChangeState(new State_Idle());
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
	if (wolf->silverWolfModel[0]) {
		wolf->silverWolfModel[0]->ResetAnimation();
		wolf->current_animation_index = 0;
		wolf->state = "idle";
		wolf->local_anim_time = 0.0f;
	}
}
void State_Idle::Update(silver_wolf* wolf, float detatime) {


	if((wolf->w_press|| wolf->a_press|| wolf->s_press|| wolf->d_press)&&wolf->shift_press&& !wolf->camera_right_mouth)
	{
		wolf->ChangeState(new State_Run());
	}
	else if ((wolf->w_press || wolf->a_press || wolf->s_press || wolf->d_press) &&!wolf->camera_right_mouth)
	{
		wolf->ChangeState(new State_Walk());
	}

}
void State_Idle::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[0]->Draw(shaderID,time);
}
void State_Idle::Exit(silver_wolf* wolf) {

}

//걷기 워크 walk
void State_Walk::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[1]) {
		wolf->silverWolfModel[1]->ResetAnimation();
		wolf->current_animation_index = 1;
		wolf->state = "walk";
		wolf->local_anim_time = 0.0f;
	}
}
void State_Walk::Update(silver_wolf* wolf, float detlatime) {

	

	glm::vec3 v(0.0f);

	if (wolf->w_press) v.z += wolf->walkSpeed * detlatime;
	if (wolf->s_press) v.z -= wolf->walkSpeed * detlatime;
	if (wolf->a_press) v.x += wolf->walkSpeed * detlatime;
	if (wolf->d_press) v.x -= wolf->walkSpeed * detlatime;

	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(-wolf->camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 1.0f));

	wolf->pos += v;

	wolf->old_camera_angle = wolf->camera_angle;

	//여기는 회전까지
	if (wolf->w_press && wolf->a_press) wolf->angle = -wolf->camera_angle +45.0f;
	else if (wolf->w_press && wolf->d_press) wolf->angle = -wolf->camera_angle -45.0f;
	else if (wolf->s_press && wolf->a_press) wolf->angle = -wolf->camera_angle+135.0f;
	else if (wolf->s_press && wolf->d_press) wolf->angle = -wolf->camera_angle -135.0f;
	else if (wolf->w_press) wolf->angle = -wolf->camera_angle+0.0f;
	else if (wolf->s_press) wolf->angle = -wolf->camera_angle+180.0f;
	else if (wolf->a_press) wolf->angle = -wolf->camera_angle+90.0f;
	else if (wolf->d_press) wolf->angle = -wolf->camera_angle-90.0f;

	wolf->silver_angle = wolf->angle + wolf->camera_angle;

	if(wolf->camera_right_mouth)wolf->ChangeState(new State_Idle());
	else if (!wolf->w_press && !wolf->a_press && !wolf->s_press && !wolf->d_press)
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
	if (press_num > 2 || (wolf->w_press && wolf->s_press) || (wolf->a_press && wolf->d_press)) {
		wolf->pos = wolf->old_pos;
		wolf->silverWolfModel[0]->Draw(shaderID, time);
	}
	else {
		wolf->silverWolfModel[1]->Draw(shaderID, time);
		bool isplaying;
		wolf->thisChannel->isPlaying(&isplaying);
		if (!isplaying) {

			wolf->thisChannel = soundManager.Play("walk", wolf->pos, effect_volume);
		}
	}
}
void State_Walk::Exit(silver_wolf* wolf) {
	wolf->thisChannel->stop();
}

//달리기 런 run
void State_Run::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[2]) {
		wolf->silverWolfModel[2]->ResetAnimation();
		wolf->run_timer = 0.0f;
		wolf->current_animation_index = 2;
		wolf->state = "run";
		wolf->local_anim_time = 0.0f;
	}
}
void State_Run::Update(silver_wolf* wolf, float detlatime) {

	

	wolf->run_timer += detlatime;

	glm::vec3 v(0.0f);

	if (wolf->w_press) v.z += wolf->runSpeed * detlatime;
	if (wolf->s_press) v.z -= wolf->runSpeed * detlatime;
	if (wolf->a_press) v.x += wolf->runSpeed * detlatime;
	if (wolf->d_press) v.x -= wolf->runSpeed * detlatime;

	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(-wolf->camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 1.0f));

	wolf->pos += v;

	wolf->old_camera_angle = wolf->camera_angle;

	//여기는 회전까지
	if (wolf->w_press && wolf->a_press) wolf->angle = -wolf->camera_angle + 45.0f;
	else if (wolf->w_press && wolf->d_press) wolf->angle = -wolf->camera_angle - 45.0f;
	else if (wolf->s_press && wolf->a_press) wolf->angle = -wolf->camera_angle + 135.0f;
	else if (wolf->s_press && wolf->d_press) wolf->angle = -wolf->camera_angle - 135.0f;
	else if (wolf->w_press) wolf->angle = -wolf->camera_angle + 0.0f;
	else if (wolf->s_press) wolf->angle = -wolf->camera_angle + 180.0f;
	else if (wolf->a_press) wolf->angle = -wolf->camera_angle + 90.0f;
	else if (wolf->d_press) wolf->angle = -wolf->camera_angle - 90.0f;

	wolf->silver_angle = wolf->angle + wolf->camera_angle;

	if (wolf->camera_right_mouth)wolf->ChangeState(new State_Idle());
	else if (!wolf->w_press && !wolf->a_press && !wolf->s_press && !wolf->d_press&& wolf->run_timer>=2.0f)
	{
		wolf->ChangeState(new State_Stop_Run());
	}
	else if (!wolf->w_press && !wolf->a_press && !wolf->s_press && !wolf->d_press)
	{
		wolf->ChangeState(new State_Idle());
	}
	else if (!wolf->shift_press)
	{
		wolf->ChangeState(new State_Walk());
	}
}
void State_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	int press_num = wolf->w_press + wolf->a_press + wolf->s_press + wolf->d_press;
	if (press_num > 2 || (wolf->w_press && wolf->s_press) || (wolf->a_press && wolf->d_press)) {
		wolf->pos = wolf->old_pos;
		wolf->silverWolfModel[0]->Draw(shaderID, time);
	}
	else {
		wolf->silverWolfModel[2]->Draw(shaderID, time);
		bool isplaying;
		wolf->thisChannel->isPlaying(&isplaying);
		if (!isplaying) {

			wolf->thisChannel = soundManager.Play("run", wolf->pos, effect_volume);
		}
	}
}
void State_Run::Exit(silver_wolf* wolf) {
	wolf->thisChannel->stop();

}

//던지기 쓰로우 throw
void State_Throw::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[3]) {
		wolf->silverWolfModel[3]->ResetAnimation();
		wolf->silverWolfModel[3]->throw_end = false;
		wolf->current_animation_index = 3;
		wolf->state = "throw";
		wolf->local_anim_time = 0.0f;
	}
	wolf->throw_timer = 0.0f;
	wolf->throw_start = false;

}
void State_Throw::Update(silver_wolf* wolf, float detlatime) {
	if (wolf->silverWolfModel[3]->throw_end)
		wolf->ChangeState(new State_Idle());
	wolf->throw_timer += detlatime;
	if (wolf->throw_timer >= wolf->throw_time && wolf->throw_press&& !wolf->throw_start) {
		wolf->ball.emplace_back(Ball(wolf->p0, wolf->p1, wolf->p2, false));
		wolf->ball.back().Init();
		wolf->ball.back().thisChannel = soundManager.Play("throw", wolf->ball.back().current_pos,effect_volume);
		wolf->throw_start = true;
	}

}
void State_Throw::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[3]->Draw(shaderID, time);
}
void State_Throw::Exit(silver_wolf* wolf) {
	wolf->throw_press = false;
}

//구르기 롤 roll
void State_Roll::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[4]) {
		wolf->silverWolfModel[4]->ResetAnimation();
		wolf->silverWolfModel[4]->roll_end = false;
		wolf->current_animation_index = 4;
		wolf->state = "roll";
		wolf->local_anim_time = 0.0f;
		
	}
}
void State_Roll::Update(silver_wolf* wolf, float detlatime) {
	bool isplaying;
	wolf->thisChannel->isPlaying(&isplaying);
	if (wolf->roll_timer< wolf->roll_time) {
		if(!isplaying)
		wolf->thisChannel = soundManager.Play("run", wolf->pos, effect_volume);
		wolf->roll_timer += detlatime;
	}
	else {
		if (!isplaying && !wolf->roll_on) {
			wolf->thisChannel = soundManager.Play("roll", wolf->pos, effect_volume);
			wolf->roll_on = true;
		}

	}
	glm::vec3 v = { 0.0f,0.0f,1.0f };
	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(wolf->angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 0.0f));
	wolf->pos += v * 4.0f * detlatime;


	if (wolf->silverWolfModel[4]->roll_end)
		wolf->ChangeState(new State_Idle());
}
void State_Roll::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[4]->Draw(shaderID, time);
}
void State_Roll::Exit(silver_wolf* wolf) {
	wolf->thisChannel->stop();
	wolf->roll_timer = 0.0f;
	wolf->roll_on = false;
}

//점프 뛰기 jump
void State_Jump::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[5]) {
		wolf->silverWolfModel[5]->ResetAnimation();
		wolf->current_animation_index = 5;
		wolf->silverWolfModel[5]->jump_end = false;
		wolf->state = "jump";
		wolf->local_anim_time = 0.0f;
	}
}
void State_Jump::Update(silver_wolf* wolf, float detlatime) {

	glm::vec3 v = { 0.0f,0.0f,1.0f };
	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(wolf->angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 0.0f));
	wolf->pos += v * wolf->walkSpeed * detlatime;

	if (wolf->silverWolfModel[5]->jump_end)
		wolf->ChangeState(new State_Walk());
}
void State_Jump::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[5]->Draw(shaderID, time);
}
void State_Jump::Exit(silver_wolf* wolf) {
}

//달리면서 점프 Run Jump
void State_Jump_Run::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[6]) {
		wolf->silverWolfModel[6]->ResetAnimation();
		wolf->current_animation_index = 6;
		wolf->silverWolfModel[6]->jump_end = false;
		wolf->local_anim_time = 0.0f;
	}
}
void State_Jump_Run::Update(silver_wolf* wolf, float detlatime) {

	glm::vec3 v = { 0.0f,0.0f,1.0f };
	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(wolf->angle), glm::vec3(0.0f, 1.0f, 0.0f));
	v = glm::vec3(m * glm::vec4(v, 0.0f));
	wolf->pos += v * wolf->runSpeed * detlatime;

	if (wolf->silverWolfModel[6]->jump_end)
		wolf->ChangeState(new State_Run());
}
void State_Jump_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[6]->Draw(shaderID, time);
}
void State_Jump_Run::Exit(silver_wolf* wolf) {
}

//달리기 멈추기 Stop Run
void State_Stop_Run::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[7]) {
		wolf->silverWolfModel[7]->ResetAnimation();
		wolf->silverWolfModel[7]->run_end = false;
		wolf->current_animation_index = 7;
		wolf->local_anim_time = 0.0f;
	}
}
void State_Stop_Run::Update(silver_wolf* wolf, float detlatime) {
	if (wolf->silverWolfModel[7]->run_end)
		wolf->ChangeState(new State_Idle());
}
void State_Stop_Run::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[7]->Draw(shaderID, time);
}
void State_Stop_Run::Exit(silver_wolf* wolf) {
}

//점프에서 멈춰있는 상태 Jump Idle
void State_Jump_Idle::Enter(silver_wolf* wolf) {
	if (wolf->silverWolfModel[8]) {
		wolf->silverWolfModel[8]->ResetAnimation();
		wolf->current_animation_index = 8;
		wolf->silverWolfModel[8]->jump_end = false;
		wolf->local_anim_time = 0.0f;
	}
}
void State_Jump_Idle::Update(silver_wolf* wolf, float detlatime) {
	if (wolf->silverWolfModel[8]->jump_end)
		wolf->ChangeState(new State_Idle());
}
void State_Jump_Idle::Draw(silver_wolf* wolf, GLuint shaderID, float time) {
	wolf->silverWolfModel[8]->Draw(shaderID, time);
}
void State_Jump_Idle::Exit(silver_wolf* wolf) {
}


