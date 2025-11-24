#pragma once
#include "animated_model.h"
class silver_wolf
{
public:

	NewModel* silverWolfModel[9];
	glm::vec3 pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
	glm::vec3 scale = glm::vec3(0.01f);
	float angle = 0.0f;
	glm::mat4 modelMat = glm::mat4(1.0f);
	int silver_wolf_fbx_size = 9;
	string state = "idle";

	// OBB 민용
	OBB silverwolf_local_obb;
	OBB silverwolf_world_obb;
	void set_obb(); // OBB 설정 함수

	//여기서부터는 속도 관련된거
	float walkSpeed = 2.0f; // 걷기 속도


	//여기서부터는 키 관련되게
	bool w_press = false;
	bool a_press = false;
	bool s_press = false;
	bool d_press = false;


public:
	silver_wolf();

	void Init();

	void Update(float deltatime);

	void State(string state, float deltatime);
	void StateChange();

	void Draw(GLuint shaderID, float currentTime);

	void Keyboard(unsigned char key, int x, int y);

	void Keyupboard(unsigned char key, int x, int y);

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

