#include "logo_mode.h"


void logo_mode::Init() {
	// 로고 모드 초기화 코드 작성
	stbi_set_flip_vertically_on_load(true);
	uiShaderProgram = LoadShader("vertex_image.glsl", "fragment_image.glsl");

	glm::vec2 size1 = glm::vec2((float)winWidth, (float)winHeight);
	glm::vec2 pos1 = glm::vec2((float)winWidth / 2.0f, (float)winHeight / 2.0f);

	background_Image = new Image(LoadTexture("logo/black_background.png"), pos1, size1);
	background_Image->color.w = 1.0f;


	tuk_image = new Image(LoadTexture("logo/tuk_credit.png"), pos1, size1);
	tuk_image->color.w = 0.0f;


	logo_Image = new Image(LoadTexture("logo/logo.png"), pos1, size1);
	logo_Image->color.w = 1.0f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

void logo_mode::Update(float deltaTime) {
	// 로고 모드 업데이트 코드 작성
	if (!turn_image) {
		tuk_image->color.w += deltaTime / 8.0f;
		if (tuk_image->color.w >= 1.0f) {
			turn_image = true;
			bgmChannel = soundManager.Play("logo", glm::vec3(0.0f, 0.0f, 0.0f), bgm_volume);
		}
	}
	else if (turn_image&& logo_timer < logo_time) {
		logo_timer += deltaTime;
	}
	else if (g_Framework && g_Framework->sceneManager) {
		stbi_set_flip_vertically_on_load(false);
		g_Framework->sceneManager->Change_Mode(new title_mode());
	}
	soundManager.Update();
}

void logo_mode::Draw() {
	// 로고 모드 그리기 코드 작성
	glm::mat4 uiProj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
	if (background_Image) background_Image->Draw(uiShaderProgram, uiProj);
	if (!turn_image) tuk_image->Draw(uiShaderProgram, uiProj);
	if (turn_image) logo_Image->Draw(uiShaderProgram, uiProj);

}

void logo_mode::OnPause() {
	// 로고 모드 일시정지 시 처리할 코드 작성
}

void logo_mode::OnResume() {
	// 로고 모드 재개 시 처리할 코드 작성
}

void logo_mode::Finish() {
	// 로고 모드 종료 시 처리할 코드 작성
}

void logo_mode::Reshape(int w, int h) {
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;
	glViewport(0, 0, w, h);
}

void logo_mode::Mouse(int button, int state, int x, int y) {
	// 로고 모드 마우스 이벤트 처리 코드 작성
	if (!turn_image) {
		turn_image = true;
		bgmChannel = soundManager.Play("logo", glm::vec3(0.0f, 0.0f, 0.0f), bgm_volume);
	}
}

void logo_mode::Keyboard(unsigned char key, int x, int y) {
	// 로고 모드 키보드 이벤트 처리 코드 작성
	if (!turn_image) {
		turn_image = true;
		bgmChannel = soundManager.Play("logo", glm::vec3(0.0f, 0.0f, 0.0f), bgm_volume);
	}
}