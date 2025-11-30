#include "game_framwork.h"
#include "game_mode.h"
#include "title_mode.h"
#include "logo_mode.h"
#include "resource.h"
GameFramework* g_Framework = nullptr;
auto* mode = new game_mode();

int main(int argc, char** argv) {
    // 1. 프레임워크 생성
    GameFramework game;

    // 2. GLUT 및 OpenGL 초기화
    game.Init(argc, argv);

    //2.5 리소스 로드
	Resource_Init();

    // 3. 시작 씬 생성 (미로 씬)
   //game_mode* startScene = new game_mode();

    // 4. 게임 루프 실행
    game.Run(mode);

    return 0;
}