#define STB_IMAGE_IMPLEMENTATION
#include "game_mode.h"

// 셰이더 파일 경로 상수
const char* STATIC_VERT = "static_vertex.glsl";
const char* ANIMATED_VERT = "animated_vertex.glsl";
const char* FRAGMENT_LIGHT = "fragment.glsl";

using namespace std;





// 생성자: 변수 초기값 설정
game_mode::game_mode() {


    lightPos = glm::vec3(0.0f, 2000.0f, 0.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    materialSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    ambientStrength = 0.1f;
    shininess = 32;
    
}

game_mode::~game_mode() {
    // Finish()가 호출되지 않고 파괴될 경우를 대비
    // (일반적으로는 Finish에서 정리함)
}

// 1. 초기화 (기존 init 함수 내용)
void game_mode::Init() {


    // GLEW 초기화는 보통 Framework나 Main에서 한 번 하지만, 
    // 여기서 셰이더 컴파일을 수행합니다.

    cout << "[GameMode] Initializing..." << endl;

    // 셰이더 로드
    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);

    // 모델 로드
    loadModels();

    // 카메라 위치
    /*camPos = glm::vec3(start_x_pos, 5.0f, start_z_pos - 5.0f);
    camTarget = glm::vec3(start_x_pos, 0.0f, start_z_pos+5.0f);*/
    glm::vec3 targetPos = silverWolf.pos;
    
    
    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	gameCamera.Init(targetPos);

    
    glEnable(GL_DEPTH_TEST);
}

float deltatime;
// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void game_mode::Update(float deltaTime) {
    silverWolf.Update(deltaTime,gameCamera.camera_x_angle, gameCamera.camera_y_angle, gameCamera.right_mouth);
    gameCamera.Update(deltaTime, silverWolf.pos);
    silverwolf_maze_collision();
    //카메라 고정
	if (camera_fixed == false) glutWarpPointer(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
	
	deltatime = deltaTime;
}

void game_mode::silverwolf_maze_collision() {

    for (auto& block : maze.mazeBlocks) {
        block.is_colliding = false;
        if (check_collision(silverWolf.silverwolf_world_obb, block.road_world_obb)) {
            block.is_colliding = true;
        }
    }

    // 청크 단위 충돌
    bool stop = false;
    for (int i = 1; i < maze_y - 1; i++) {
        for (int j = 1; j < maze_x - 1; j++) {
            if (maze.mazeBlocks[(i * maze_x) + j].is_colliding == true) {
                update_chunk(i, j, 2);
                stop = true;
                break;
            }
        }
        if (stop) break;
    }

    // 청크 기준으로 장애물 충돌 검사
    for (auto& block : maze.mazeBlocks) {
        if (block.is_colliding == true) {
            for (int j = 0; j < block.obstacle_world_obb.size(); j++) {
                if (check_collision(silverWolf.silverwolf_world_obb, block.obstacle_world_obb[j])) {
                    silverWolf.pos = silverWolf.old_pos;
                    silverWolf.update_world_obb();
                }
            }
        }
    }
}

void game_mode::update_chunk(int y, int x, int size) {
    int min_y = std::max(0, y - size);
    int max_y = std::min(maze_y, y + size);
    int min_x = std::max(0, x - size);
    int max_x = std::min(maze_x, x + size);

    for (int i = min_y; i <= max_y; i++) {
        for (int j = min_x; j <= max_x; j++) {
            maze.mazeBlocks[(i * maze_x) + j].is_colliding = true;
        }
    }
}

// 3. 그리기 (기존 drawScene 함수 내용)
void game_mode::Draw() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 뷰, 프로젝션 행렬 계산
    glm::mat4 view = glm::lookAt(gameCamera.camPos, gameCamera.camTarget, gameCamera.camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // --- 1. 정적 오브젝트 (미로, 바닥) ---
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    // 미로 그리기는 initmaze 내부에서 설정된 roads 벡터나 
    // mazeBlocks를 통해 그려질 것으로 추정됩니다.
    // 기존 main.cpp에는 주석처리 되어있었으나, 
    // 실제로는 roads에 있는 모델들이 어딘가에서 그려져야 합니다.
    // (만약 roads 벡터를 순회하며 그려야 한다면 아래 코드 사용)
    for (auto& block : maze.mazeBlocks) {
        if (block.is_colliding) {
            glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(block.modelMatrix));
            if (block.modelPtr)
            {
                // 메시별 재질/색상 정보 설정 및 드로우
                for (auto& mesh : block.modelPtr->meshes)
                {
                    // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
                    mesh.Draw(shaderProgramStatic);
                }
            }
        }
    }

    //glDisable(GL_DEPTH_TEST); // OBB가 겹쳐도 항상 보이게 (선택 사항)
    glLineWidth(3.0f);        // 선 굵기 설정

    for (auto& block : maze.mazeBlocks) {
        if (block.is_colliding) {
            if (block.modelPtr) {
                drawDebugOBB(shaderProgramStatic, block.road_world_obb, view, proj, glm::vec3(0.0f, 1.0f, 0.0f)); // 초록색

                for (int i = 0; i < block.obstacle_world_obb.size(); i++)
                    drawDebugOBB(shaderProgramStatic, block.obstacle_world_obb[i], view, proj, glm::vec3(1.0f, 1.0f, 0.0f)); // 노란색
            }
        }
    }

    //glEnable(GL_DEPTH_TEST); // DEPTH TEST 복원

    // --- 2. 애니메이션 캐릭터 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
    silverWolf.Draw(shaderProgramAnimated, deltatime, view,  proj, glm::vec3(1.0f, 0.0f, 1.0f));
    if (!silverWolf.init_success) silverWolf.init_success = true;
    drawDebugOBB(shaderProgramStatic, silverWolf.silverwolf_world_obb, view, proj, glm::vec3(1.0f, 0.0f, 0.0f)); // 빨간색
}

void game_mode::Keyboard(unsigned char key, int x, int y) {

    silverWolf.Keyboard(key, x, y);

    switch (key)
    {
    case'g':
    case'G':
        camera_fixed = true;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        break;
    }

}
void game_mode::Keyupboard(unsigned char key, int x, int y) {

    silverWolf.Keyupboard(key, x, y);

    switch (key)
    {
    case 27:
        exit(0);
        break;
    case'g':
    case'G':
        camera_fixed = false;
        //glutSetCursor(GLUT_CURSOR_NONE);
        break;
    }
}

void game_mode::SpecialKeyboard(int key, int x, int y) {
    silverWolf.SpecialKeyboard(key, x, y);


}

void game_mode::SpecialUpKeyboard(int key, int x, int y) {
    silverWolf.SpecialUpKeyboard(key, x, y);
}

void game_mode::Mouse(int button, int state, int x, int y) {
    gameCamera.Mouse(button, state, x, y);
    silverWolf.Mouse(button, state, x, y);
}

void game_mode::PassiveMotion(int x, int y) {
    if (silverWolf.init_success)
        gameCamera.PassiveMotion(x, y, camera_fixed);
}

void game_mode::set_taret_in_maze() {  // 과녁에서 미로 객체를 가져올 수 없어서 여기서 배치해줌
    //ㅗ
}
void  game_mode::Motion(int x, int y) {
    gameCamera.Motion(x, y, camera_fixed);
}

// 4. 정리 (종료 시 메모리 해제)
void game_mode::Finish() {
    // 로드된 도로 모델들 삭제
    for (auto p : roads) {
        delete p;
    }
    roads.clear();

    // 늑대 모델 삭제 (silver_wolf 클래스 내부 구조에 따라 다를 수 있음)
    // NewModel* 포인터들을 가지고 있다면 여기서 delete 해주는 것이 좋음
    for (int i = 0; i < 7; ++i) {
        if (silverWolf.silverWolfModel[i])
            delete silverWolf.silverWolfModel[i];
    }

    // 셰이더 프로그램 삭제
    glDeleteProgram(shaderProgramStatic);
    glDeleteProgram(shaderProgramAnimated);
}

void game_mode::Reshape(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    glViewport(0, 0, w, h);
}

void game_mode::OnPause() {
    // 옵션 창 등을 열었을 때 멈춰야 할 로직이 있다면 여기에 작성
}

void game_mode::OnResume() {
    // 옵션 창 닫고 돌아왔을 때 복구할 로직
    glEnable(GL_DEPTH_TEST); // 혹시 다른 씬에서 껐을까봐 다시 켬
}

// ==========================================================
// [Helper Functions] 기존 main.cpp의 함수들을 멤버 함수로 이식
// ==========================================================

void game_mode::loadModels() {
    // 미로
    roads.push_back(new StaticModel("road/road0.obj"));   // 0동
    roads.push_back(new StaticModel("road/road1.obj"));   // 1서 
    roads.push_back(new StaticModel("road/road2.obj"));   // 2남 
    roads.push_back(new StaticModel("road/road3.obj"));   // 3북
    roads.push_back(new StaticModel("road/road4.obj"));   // 4ㅡ
    roads.push_back(new StaticModel("road/road5.obj"));   // 5ㅣ
    roads.push_back(new StaticModel("road/road6.obj"));   // 6┌
    roads.push_back(new StaticModel("road/road7.obj"));   // 7┐
    roads.push_back(new StaticModel("road/road8.obj"));   // 8└
    roads.push_back(new StaticModel("road/road9.obj"));   // 9┘
    roads.push_back(new StaticModel("road/road10.obj"));  // 10ㅏ
    roads.push_back(new StaticModel("road/road11.obj"));  // 11ㅓ
    roads.push_back(new StaticModel("road/road12.obj"));  // 12ㅜ
    roads.push_back(new StaticModel("road/road13.obj"));  // 13ㅗ
    roads.push_back(new StaticModel("road/road14.obj"));  // 14+
    roads.push_back(new StaticModel("road/road15.obj"));  // 15x
    for(int i = 0; i < roads.size(); i++) {
        roads[i]->set_maze_obb(i);
	}
    maze.setMaze();
    maze.initmaze(&roads);

    //과녁
	target_model = new StaticModel("target/cube.obj");
    target.init(target_model,target_count);
    set_taret_in_maze();    //미로에 과녁 배치

    //은랑
    silverWolf.Init();

    silverWolf.silverWolfModel[0] = new NewModel("silver_wolf/Idle.fbx");
    silverWolf.silverWolfModel[0]->state = "idle";
    silverWolf.silverWolfModel[1] = new NewModel("silver_wolf/Walk.fbx");
    silverWolf.silverWolfModel[1]->state = "walk";
    silverWolf.silverWolfModel[2] = new NewModel("silver_wolf/Running.fbx");
    silverWolf.silverWolfModel[2]->state = "run";
    silverWolf.silverWolfModel[3] = new NewModel("silver_wolf/Throw.fbx");
    silverWolf.silverWolfModel[3]->state = "throw";
    silverWolf.silverWolfModel[4] = new NewModel("silver_wolf/Jump Over.fbx");
    silverWolf.silverWolfModel[4]->state = "roll";
    silverWolf.silverWolfModel[5] = new NewModel("silver_wolf/Jump.fbx");
    silverWolf.silverWolfModel[5]->state = "jump";
    silverWolf.silverWolfModel[6] = new NewModel("silver_wolf/Running Jump.fbx");
    silverWolf.silverWolfModel[6]->state = "jump_run";
    silverWolf.silverWolfModel[7] = new NewModel("silver_wolf/Run To Stop.fbx");
    silverWolf.silverWolfModel[7]->state = "stop_run";
    silverWolf.silverWolfModel[8] = new NewModel("silver_wolf/Backflip.fbx");
    silverWolf.silverWolfModel[8]->state = "jump_idle";


}

void game_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(gameCamera.camPos));

    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);

    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
}

char* game_mode::filetobuf(const char* file) {
    ifstream f(file, ios::binary);
    if (!f.is_open()) {
        cerr << "ERROR: Cannot open shader file: " << file << endl;
        return nullptr;
    }
    f.seekg(0, ios::end);
    int len = (int)f.tellg();
    char* buf = new char[len + 1];
    f.seekg(0, ios::beg);
    f.read(buf, len);
    buf[len] = '\0';
    f.close();
    return buf;
}

void game_mode::loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
    GLuint vertShader, fragShader;
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    char* vertCode = filetobuf(vertPath);
    if (!vertCode) return;
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCode, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        cerr << "ERROR::VERTEX::COMPILATION_FAILED: " << vertPath << "\n" << infoLog << endl;
    }
    delete[] vertCode;

    // Fragment Shader
    char* fragCode = filetobuf(fragPath);
    if (!fragCode) { glDeleteShader(vertShader); return; }
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        cerr << "ERROR::FRAGMENT::COMPILATION_FAILED: " << fragPath << "\n" << infoLog << endl;
    }
    delete[] fragCode;

    // Program Link
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertShader);
    glAttachShader(shaderID, fragShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}



