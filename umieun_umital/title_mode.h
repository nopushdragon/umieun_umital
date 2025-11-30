#pragma once
#include "scene_manager.h"
#include "static_model.h"
#include "maze.h"
#include "silver_wolf.h"
#include "camera.h"
#include "skybox.h"

class title_mode : public Scene
{
public:
	silver_wolf silverWolf;

private:
	GLuint shaderProgramStatic;
	GLuint shaderProgramAnimated;
	GLuint shaderProgramSkybox;

	// 모델 데이터
	std::vector<StaticModel*> roads;
	MAZE maze;

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

	// --- 내부 헬퍼 함수 (셰이더 로딩용) ---
	char* filetobuf(const char* file);
	void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID);
	void setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj);
	void loadModels();

	const char* STATIC_VERT = "static_vertex.glsl";
	const char* ANIMATED_VERT = "animated_vertex.glsl";
	const char* FRAGMENT_LIGHT = "fragment.glsl";
public:
	title_mode();
	~title_mode();

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

