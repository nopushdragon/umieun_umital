#pragma once
#include "scene_manager.h"
#include "static_model.h"
#include "maze.h"
#include "silver_wolf.h"
#include "collision.h"

class game_mode : public Scene
{
private:
    GLuint shaderProgramStatic;
    GLuint shaderProgramAnimated;

    // 모델 데이터
    silver_wolf silverWolf;
    std::vector<StaticModel*> roads;
    // StaticModel* wallModel; // main.cpp에서 nullptr로 초기화만 되고 사용되지 않아 주석 처리 (필요시 해제)

    // 카메라 및 화면 설정
    int WINDOW_WIDTH = 1200;
    int WINDOW_HEIGHT = 800;
    glm::vec3 camPos;
    glm::vec3 camTarget;
    glm::vec3 camUp;

    // 조명 및 재질 설정
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 materialSpecular;
    float ambientStrength;
    int shininess;

    // --- 내부 헬퍼 함수 (셰이더 로딩용) ---
    char* filetobuf(const char* file);
    void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID);
    void setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj);
    void loadModels();
    void drawDebugOBB(GLuint shaderID, const OBB& obb, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& color);
public:
    game_mode();
    ~game_mode();
	void Init() override;
	void Update(float deltaTime) override;

	void OnPause() override;
	void OnResume() override;
	void Draw() override;
	void Finish() override;
	void Reshape(int w, int h) override;
    void Keyboard(unsigned char key, int x, int y) override;

    void Keyupboard(unsigned char key, int x, int y) override;
    void SpecialKeyboard(int key, int x, int y) override;
    void SpecialUpKeyboard(int key, int x, int y) override;

	/*void Mouse(int button, int state, int x, int y) override;
	void MouseWheel(int wheel, int direction, int x, int y) override;
	void Motion(int x, int y) override;
	void PassiveMotion(int x, int y) override;
	void Keyboard(unsigned char key, int x, int y) override;
	void Keyupboard(unsigned char key, int x, int y) override;

	void SpecialKeyboard(int key, int x, int y) override;
	void SpecialUpKeyboard(int key, int x, int y) override;*/
    
};