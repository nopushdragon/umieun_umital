#pragma once
#include "scene_manager.h"
#include "static_model.h"
#include "maze.h"
#include "silver_wolf.h"
#include "camera.h"
#include "image.h"
#include "text.h"
#include "skybox.h"
#include "trainer.h"
#include "chest.h"

class clear_mode : public Scene
{
public:
	silver_wolf silverWolf;

private:
	GLuint shaderProgramStatic;
	GLuint shaderProgramAnimated;
	GLuint shaderProgramImage;
	GLuint shaderProgramSkybox;
	GLuint shaderProgramText;

	//scene 진행도
	int scene_progress = 0;
	int event_progress = 0;
	bool pause = false;
	bool qte_pause = false;
	int qte_success_count = 0;
	int qte_is_success = 0;	// 0: 아직 안함, 1: 성공, -1: 실패

	// 모델 데이터
	vector<MazeBlockInstance> mazeBlocks;
	chestInstance chest;
	Trainer* trainer = nullptr;
	void objects_Update(float deltaTime);

	//ui 및 이미지 관련
	Image* black_background = nullptr;
	Image* black_bar = nullptr;
	Image* qte_f = nullptr;
	Image* qte_tip = nullptr;
	vector<Image*> scene_1;
	vector<Image*> scene_2;

	void drawCircle(float x, float y, float r, glm::vec4 color);
	float circle_radius = 200.0f;

	void reshape_ui(float w, float h);
	glm::vec2 ui_dis = glm::vec2(0.0f, 0.0f);


	// 카메라 및 화면 설정
	camera gameCamera;
	int WINDOW_WIDTH = winWidth;
	int WINDOW_HEIGHT = winHeight;
	//카메라고정풀기
	bool camera_fixed = false;
	int start_pos_idx = 0;
	int end_pos_idx = 0;

	// 조명 및 재질 설정
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	glm::vec3 materialSpecular;
	float ambientStrength;
	int shininess;

	Skybox* skybox = nullptr;

	void Fog_Update();


	// --- 내부 헬퍼 함수 (셰이더 로딩용) ---
	char* filetobuf(const char* file);
	void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID);
	void setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj);
	void loadModels();
	void loadImages();
	void loadTexts();

	const char* STATIC_VERT = "static_vertex.glsl";
	const char* ANIMATED_VERT = "animated_vertex.glsl";
	const char* FRAGMENT_LIGHT = "fragment.glsl";
	const char* IMAGE_VERT = "vertex_image.glsl";
	const char* IMAGE_FRAG = "fragment_image.glsl";
	const char* TEXT_VERT = "vertex_text.glsl";
	const char* TEXT_FRAG = "fragment_text.glsl";

public:
	clear_mode();
	~clear_mode();

	void Init() override;
	void Update(float deltaTime) override;
	void OnPause() override;
	void OnResume() override;
	void Draw() override;
	void Finish() override;
	void Reshape(int w, int h) override;

	void Mouse(int button, int state, int x, int y) override;
	void Motion(int x, int y) override;
	void PassiveMotion(int x, int y) override;
	void Keyboard(unsigned char key, int x, int y) override;
	void Keyupboard(unsigned char key, int x, int y) override;

	void SpecialKeyboard(int key, int x, int y) override;
	void SpecialUpKeyboard(int key, int x, int y) override;
};

