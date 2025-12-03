#include "option_mode.h"
#include "game_framwork.h"

void option_mode::Init() {
    // 옵션 모드 초기화 코드 작성
}

void option_mode::Update(float deltaTime) {
    // 옵션 모드 업데이트 코드 작성
}

void option_mode::Draw() {
    // 옵션 모드 그리기 코드 작성
}

void option_mode::OnPause() {
    // 옵션 모드 일시정지 시 처리할 코드 작성
}

void option_mode::OnResume() {
    // 옵션 모드 재개 시 처리할 코드 작성
}

void option_mode::Finish() {
    // 옵션 모드 종료 시 정리할 코드 작성
}

void option_mode::Reshape(int w, int h) {
    // 옵션 모드 창 크기 변경 시 처리할 코드 작성
}

void option_mode::Mouse(int button, int state, int x, int y) {
    // 옵션 모드 마우스 클릭 시 처리할 코드 작성
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