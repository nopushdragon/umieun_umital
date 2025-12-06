#include "headers.h"
#include "game_framwork.h"
#include "option_mode.h"
#include "resource.h"

void option_mode::Init() {
    // 옵션 모드 초기화 코드 작성
    shaderProgramImage = LoadShader("vertex_image.glsl", "fragment_image.glsl");
	shaderProgramText = LoadShader("vertex_text.glsl", "fragment_text.glsl");
	stbi_set_flip_vertically_on_load(true);

	glm::vec2 size1 = glm::vec2((float)winWidth, (float)winHeight);
	glm::vec2 pos1 = glm::vec2((float)winWidth / 2.0f, (float)winHeight / 2.0f);

	black_background = new Image(LoadTexture("scene_image/black_background.png"), pos1, size1);
	black_background->color.w = 0.5f;

	option_background = new Image(LoadTexture("scene_image/option.png"), pos1, size1);
	option_background->color.w = 1.0f;

    for (int i = 0; i < 10;i++) {
        bgm_volume_image[i] = new Image(LoadTexture("scene_image/volum_bar.png"), glm::vec2(480.0f + 24.0f + 48.0f*i, 475.0f), glm::vec2(48.0f, 110.0f));
        effect_volume_image[i] = new Image(LoadTexture("scene_image/volum_bar.png"), glm::vec2(480.0f + 24.0f + 48.0f * i, 245.0f), glm::vec2(48.0f, 110.0f));;
    }

}

void option_mode::Finish() {
    // 옵션 모드 종료 시 정리할 코드 작성
	delete black_background;
    delete option_background;
    for (int i = 0; i < 10; i++) {
        delete bgm_volume_image[i];
        delete effect_volume_image[i];
	}

    if (shaderProgramImage) glDeleteProgram(shaderProgramImage);
    if (shaderProgramText) glDeleteProgram(shaderProgramText);
    stbi_set_flip_vertically_on_load(false);
}

void option_mode::Update(float deltaTime) {
    // 옵션 모드 업데이트 코드 작성
}

void option_mode::Draw() {
    // 옵션 모드 그리기 코드 작성

    // --- 3. UI 그리기 ---
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 uiProj = glm::ortho(0.0f, (float)winWidth, 0.0f, (float)winHeight);

	if (black_background) black_background->Draw(shaderProgramImage, uiProj);
	if (option_background) option_background->Draw(shaderProgramImage, uiProj);

    for (int i = 0; i < bgm_volume*10; i++) {
        bgm_volume_image[i]->Draw(shaderProgramImage, uiProj);
	}
    for (int i = 0; i < effect_volume*10; i++) {
        effect_volume_image[i]->Draw(shaderProgramImage, uiProj);
	}

	mouth_image->Draw(shaderProgramImage, uiProj);
   
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void option_mode::OnPause() {
    // 옵션 모드 일시정지 시 처리할 코드 작성
}

void option_mode::OnResume() {
    // 옵션 모드 재개 시 처리할 코드 작성
}


void option_mode::Reshape(int w, int h) {
    // 옵션 모드 창 크기 변경 시 처리할 코드 작성
}

void option_mode::Mouse(int button, int state, int x, int y) {
    // 옵션 모드 마우스 클릭 시 처리할 코드 작성
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // BGM 볼륨 조절 영역
        if (x >= 330 && x <= 445  && y >= 265 && y <= 385) {
            bgm_volume -= 0.1f;
			if (bgm_volume <= 0.0f) bgm_volume = 0.0f;
            bgmChannel->setVolume(bgm_volume);
        }
        else if (x >= 990 && x <= 1105 && y >= 265 && y <= 385) {
            bgm_volume += 0.1f;
			if (bgm_volume >= 1.0f) bgm_volume = 1.0f;
			bgmChannel->setVolume(bgm_volume);
        }
        // 효과음 볼륨 조절 영역
        else if (x >= 330 && x <= 445 && y >= 495 && y <= 615) {
            effect_volume -= 0.1f;
			if (effect_volume <= 0.0f) effect_volume = 0.0f;
        }
        else if (x >= 990 && x <= 1105 && y >= 495 && y <= 615) {
			effect_volume += 0.1f;
			if (effect_volume >= 1.0f) effect_volume = 1.0f;
        }
		cout << "mouse x: " << x << ", y: " << y << endl;
		cout << "BGM Volume: " << bgm_volume << ", Effect Volume: " << effect_volume << endl;
	}
    mouth_image->position = glm::vec2((float)x + 32, (float)(WINDOW_HEIGHT - (y + 32)));

}

void option_mode::MouseWheel(int wheel, int direction, int x, int y) {
    // 옵션 모드 마우스 휠 시 처리할 코드 작성
}

void option_mode::Motion(int x, int y) {
    // 옵션 모드 마우스 움직임 시 처리할 코드 작성
}

void option_mode::PassiveMotion(int x, int y) {
    // 옵션 모드 마우스 패시브 움직임 시 처리할 코드 작성
}

void option_mode::Keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 27:    // esc
		g_Framework->sceneManager->Pop_Mode();
        break;
    }
}

void option_mode::Keyupboard(unsigned char key, int x, int y) {
    // 옵션 모드 키보드 뗐을 때 처리할 코드 작성
}

void option_mode::SpecialKeyboard(int key, int x, int y) {
    // 옵션 모드 특수키 눌렀을 때 처리할 코드 작성
}

void option_mode::SpecialUpKeyboard(int key, int x, int y) {
    // 옵션 모드 특수키 뗐을 때 처리할 코드 작성
}