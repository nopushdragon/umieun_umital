#pragma once
#include "animated_model.h"
#include "ball.h"
#include "collision.h"


class WolfState;

class silver_wolf
{
private:
	WolfState* currentState;

	float max_rock_distance = 10.0f;
	float max_angle_difference = 30.0f;
	float min_rock_distance = 1.0f;
	float rock_distance = max_rock_distance - min_rock_distance;
	

	glm::vec3 rock_end_pos = glm::vec3(0.0f, 0.0f, 0.0f);


	
	array<glm::vec3, 500> rock_path;

	
public:

	NewModel* silverWolfModel[9];
	int silver_wolf_fbx_size = 9;
	int current_animation_index = 0;
	glm::vec3 pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	glm::vec3 old_pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	glm::vec3 scale = glm::vec3(0.005f);
	float angle = 0.0f;
	glm::mat4 modelMat = glm::mat4(1.0f);
	string state = "idle";
	bool camera_right_mouth = false;

	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;

	vector<Ball> ball;

	// OBB 민용
	OBB silverwolf_local_obb;
	OBB silverwolf_world_obb;
	void set_obb(); // OBB 설정 함수
	void update_world_obb();

	//여기서부터는 속도 관련된거
	float walkSpeed = 1.0f; // 걷기 속도
	float runSpeed = 3.0f; // 걷기 속도


	//여기서부터는 키 관련되게
	//기본 wasd
	bool w_press = false;
	bool a_press = false;
	bool s_press = false;
	bool d_press = false;

	//달리기
	bool shift_press = false;

	//구르기 얜 안씀
	bool f_press = false;

	//던지기
	bool throw_press = false;

	//공 던지는 타이머
	bool throw_start = false;
	float throw_timer = 0.0f;
	float throw_time = 0.8f;



	//타이머
	float run_timer = 0.0f;
	float d_w_timer = 0.0f;
	float d_a_timer = 0.0f;
	float d_s_timer = 0.0f;
	float d_d_timer = 0.0f;
	
	//초기화 성공
	bool init_success = false;

	//x앵글
	float camera_angle = 0.0f;
	float old_camera_angle = 0.0f;

	//은랑 각도
	float silver_angle = 0.0f;

	float local_anim_time = 0.0f;

	



public:
	silver_wolf();

	void Init();

	void Update(float deltatime,const float &camera_x_angle, const float& camera_y_angle, const bool& right_mouth);

	void ChangeState(WolfState* newState);

	void Draw(GLuint shaderID, float currentTime, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);

	void Ball_Draw(GLuint shaderProgramStatic, float currentTime, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);

	void Keyboard(unsigned char key, int x, int y);

	void Keyupboard(unsigned char key, int x, int y);

	void SpecialKeyboard(unsigned char key, int x, int y);

	void SpecialUpKeyboard(unsigned char key, int x, int y);

	void Mouse(int button, int state, int x, int y);

	void ForceToIdle();

	void Calculate(const float& camera_y_angle);

	void Rock_path_draw(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);

	void DebugOBB(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);

	void DebugOBB_Ball(GLuint shaderProgramStatic, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);




	~silver_wolf();



};

/*===== Animation Info : silver_wolf/Idle.fbx =====
  - 총 프레임 수 (Duration): 500 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 8.33333 초
===========================================
===== Animation Info : silver_wolf/Walk.fbx =====
  - 총 프레임 수 (Duration): 60 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 1 초
===========================================
===== Animation Info : silver_wolf/Stop Walking.fbx =====
  - 총 프레임 수 (Duration): 180 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 3 초
===========================================
===== Animation Info : silver_wolf/Running.fbx =====
  - 총 프레임 수 (Duration): 42 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 0.7 초
===========================================
===== Animation Info : silver_wolf/Run To Stop.fbx =====
  - 총 프레임 수 (Duration): 54 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 0.9 초
===========================================
===== Animation Info : silver_wolf/Throw.fbx =====
  - 총 프레임 수 (Duration): 131 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 2.18333 초
===========================================
===== Animation Info : silver_wolf/Stand To Roll.fbx =====
  - 총 프레임 수 (Duration): 142 프레임
  - 초당 프레임 (FPS): 60
  - 총 재생 시간: 2.36667 초
===========================================




*/

