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
    silverWolf.Update(deltaTime, gameCamera.camera_x_angle, gameCamera.camera_y_angle, gameCamera.right_mouth);
    gameCamera.Update(deltaTime, silverWolf.pos);
    //카메라 고정
    if (camera_fixed == false) glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    silverwolf_chunk_collision();   //청크  

    //과녁
    target.update();


    deltatime = deltaTime;
}

void game_mode::silverwolf_chunk_collision() {

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

    for (auto& c : chest.chestBlocks) {
        if (!c.is_in_chunk) continue;
        c.is_nearby = false;
        if (check_collision(silverWolf.silverwolf_world_obb, c.chest_obb)) {
            silverWolf.pos = silverWolf.old_pos;
			c.is_nearby = true;
            silverWolf.update_world_obb();
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

    for (auto& t : target.targetBlocks) {
        t.is_in_chunk = false;
        if (t.modelMatrix[3].x > (min_x * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && t.modelMatrix[3].x < ((max_x + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
            t.modelMatrix[3].y >(min_y * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && t.modelMatrix[3].y < ((max_y + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
            t.is_in_chunk = true;
        }
    }

    for (auto& c : chest.chestBlocks) {
		c.is_in_chunk = false;
        if (c.modelMatrix[3].x > (min_x * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && c.modelMatrix[3].x < ((max_x + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
            c.modelMatrix[3].y >(min_y * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && c.modelMatrix[3].y < ((max_y + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
            c.is_in_chunk = true;
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

    // static 모델 그리기
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    for (auto& block : maze.mazeBlocks) {   //미로
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

    for (auto& t : target.targetBlocks) {   //과녁
        if (t.is_break || !t.is_in_chunk) continue;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(t.modelMatrix));
        if (t.modelPtr)
        {
            // 메시별 재질/색상 정보 설정 및 드로우
            for (auto& mesh : t.modelPtr->meshes)
            {
                // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
                mesh.Draw(shaderProgramStatic);
            }
        }
    }

    for(auto& c : chest.chestBlocks) {   //보물
        if (!c.is_in_chunk) continue;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(c.modelMatrix));
        if (c.modelPtr)
        {
            // 메시별 재질/색상 정보 설정 및 드로우
            for (auto& mesh : c.modelPtr->meshes)
            {
                // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
                mesh.Draw(shaderProgramStatic);
            }
        }
	}


    // --- 2. 애니메이션 캐릭터 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
    silverWolf.Draw(shaderProgramAnimated, shaderProgramStatic, deltatime, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));
    if (!silverWolf.init_success) silverWolf.init_success = true;

    // --- 3. 오브젝트들 OBB ---
    if (collision_on) {
        glLineWidth(3.0f);        // 선 굵기 설정
        for (auto& block : maze.mazeBlocks) {   // 미로 obb
            if (block.is_colliding) {
                if (block.modelPtr) {
                    drawDebugOBB(shaderProgramStatic, block.road_world_obb, view, proj, glm::vec3(0.0f, 1.0f, 0.0f)); // 초록색

                    for (int i = 0; i < block.obstacle_world_obb.size(); i++) {
                        drawDebugOBB(shaderProgramStatic, block.obstacle_world_obb[i], view, proj, glm::vec3(1.0f, 1.0f, 0.0f)); // 노란색
                    }
                }
            }
        }

        for (auto& t : target.targetBlocks) {   // 과녁 obb
            if (t.is_break || !t.is_in_chunk) continue;
            drawDebugOBB(shaderProgramStatic, t.target_obb, view, proj, glm::vec3(0.0f, 0.0f, 1.0f)); // 파란색
        }

        for (auto& c: chest.chestBlocks) {   // 보물 obb
            if (!c.is_in_chunk) continue;
            drawDebugOBB(shaderProgramStatic, c.chest_obb, view, proj, glm::vec3(1.0f, 0.5f, 0.0f)); // 주황색
		}
		

        // 은랑 obb
        drawDebugOBB(shaderProgramStatic, silverWolf.silverwolf_world_obb, view, proj, glm::vec3(1.0f, 0.0f, 0.0f)); // 빨간색
    }
}

void game_mode::Keyboard(unsigned char key, int x, int y) {

    silverWolf.Keyboard(key, x, y);

    switch (key)
    {
    case 9:
        collision_on = !collision_on;
		break;
    case'g':
    case'G':
        camera_fixed = true;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        break;
    case 'o':
        chest.open_chest();
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

    delete target_model;
	delete chest_model;
	

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
    for (int i = 0; i < roads.size(); i++) {
        roads[i]->set_maze_obb(i);
    }
    maze.setMaze();
    maze.initmaze(&roads);

    //과녁
    target_model = new StaticModel("target/baudrive.obj");
    target_model->set_target_obb();
    target.init(target_model, target_count);
    set_taret_in_maze();    //미로에 과녁 배치

    //보물
	chest_model = new StaticModel("chest/chest.obj");
	chest_model->set_chest_obb();
	chest.init(chest_model, maze.mazeBlocks[maze_x+1].reset);

    //은랑
    silverWolf.Init();

    //몬스터볼 ㅇㅅㅇ;


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

void game_mode::set_taret_in_maze() {  // 과녁에서 미로 객체를 가져올 수 없어서 여기서 배치해줌
    uniform_int_distribution<int> rd_x(1, maze_x - 2);
    uniform_int_distribution<int> rd_y(1, maze_y - 2);

    for (int i = 0; i < target.targetBlocks.size();i++) {

        while (1) {
            int rand_x = rd_x(mt), rand_y = rd_y(mt);
            bool rd_flag = false;
            if (maze.maze[rand_y][rand_x].path_wall == WALL || maze.maze[rand_y][rand_x].type == 15 || (rand_y == 1 && rand_x == 1)) continue;
            for (int j = 0; j < i;j++) {
                if (target.targetBlocks[j].modelMatrix == maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix) {
                    rd_flag = true;
                    break;
                }
            }
            if (rd_flag) continue;

            target.targetBlocks[i].modelMatrix = maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix;
            target.targetBlocks[i].reset = glm::vec3(target.targetBlocks[i].modelMatrix[3][0], ROAD_SIZE / 4, target.targetBlocks[i].modelMatrix[3][2]); // 위치 저장
            cout << rand_y << "," << rand_x << endl;
            break;
        }
    }

}