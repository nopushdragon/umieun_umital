#pragma once
#include "scene_manager.h"
#include "static_model.h"
#include "maze.h"
#include "target.h"
#include "chest.h"
#include "silver_wolf.h"
#include "camera.h"
#include "collision.h"
#include "skybox.h"
#include "ball.h"
#include "trainer.h"


class game_mode : public Scene
{
public:
    silver_wolf silverWolf;
private:
    GLuint shaderProgramStatic;
    GLuint shaderProgramAnimated;
	GLuint shaderProgramSkybox;

    // 모델 데이터
    MAZE maze;

	int target_count = 5;
	TARGET target;

    CHEST chest;
	bool collision_on = false;  // obb 선 그리기용

    vector<Ball> balls;
	int ball_cnt = 30;

	vector<Trainer*> trainers;
  
    

   

    // 카메라 및 화면 설정
	camera gameCamera;
    int WINDOW_WIDTH = winWidth;
    int WINDOW_HEIGHT = winHeight;
    //카메라고정풀기
	bool camera_fixed = false;

    // 조명 및 재질 설정
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 materialSpecular;
    float ambientStrength;
    int shininess;

    //스벅
	Skybox* skybox = nullptr;

    //적 나오는 시간
	float trainer_spawn_timer = 0.0f;
	float trainer_spawn_time = 5.0f;

    // --- 내부 헬퍼 함수 (셰이더 로딩용) ---
    char* filetobuf(const char* file);
    void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID);
    void setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj);
    void loadModels();
public:
    game_mode();
    ~game_mode();
	void Init() override;
	void Update(float deltaTime) override;
	void silverwolf_chunk_collision();

    void trainer_chunk_collision();
    void update_chunk(int y, int x, int size);  // size: 중앙서부터 몇칸만큼의 범위로 할거냐

    void Update_Trainer_Spawn();

    void set_target_in_maze();
    void set_ball_in_maze(Ball& b);

    void set_trainer_in_maze(Trainer& t);

	void OnPause() override;
	void OnResume() override;
	void Draw() override;
	void Finish() override;
	void Reshape(int w, int h) override;
    void Keyboard(unsigned char key, int x, int y) override;

    void Keyupboard(unsigned char key, int x, int y) override;
    void SpecialKeyboard(int key, int x, int y) override;
    void SpecialUpKeyboard(int key, int x, int y) override;

    void Mouse(int button, int state, int x, int y) override;

    void PassiveMotion(int x, int y) override;
    void Motion(int x, int y) override;



	/*void Mouse(int button, int state, int x, int y) override;
	void MouseWheel(int wheel, int direction, int x, int y) override;
	void Motion(int x, int y) override;
	void PassiveMotion(int x, int y) override;
	void Keyboard(unsigned char key, int x, int y) override;
	void Keyupboard(unsigned char key, int x, int y) override;

	void SpecialKeyboard(int key, int x, int y) override;
	void SpecialUpKeyboard(int key, int x, int y) override;*/
    
};