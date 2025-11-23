#define STB_IMAGE_IMPLEMENTATION
#include "game_mode.h"

// 셰이더 파일 경로 상수
const char* STATIC_VERT = "static_vertex.glsl";
const char* ANIMATED_VERT = "animated_vertex.glsl";
const char* FRAGMENT_LIGHT = "fragment.glsl";

using namespace std;

// 생성자: 변수 초기값 설정
game_mode::game_mode() {
    camPos = glm::vec3(0.0f, 60.0f, 10.0f);
    camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

    // 미로 설정
    //setMaze();
    //initmaze(&roads);

    // 카메라 위치
    camPos = glm::vec3(start_x_pos, 25.0f, start_z_pos - 5.0f);
    camTarget = glm::vec3(start_x_pos, 20.0f, start_z_pos);

    glEnable(GL_DEPTH_TEST);
}

// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void game_mode::Update(float deltaTime) {
    
}

// 3. 그리기 (기존 drawScene 함수 내용)
void game_mode::Draw() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 뷰, 프로젝션 행렬 계산
    glm::mat4 view = glm::lookAt(camPos, camTarget, camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // --- 1. 정적 오브젝트 (미로, 바닥) ---
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    // 미로 그리기는 initmaze 내부에서 설정된 roads 벡터나 
    // mazeBlocks를 통해 그려질 것으로 추정됩니다.
    // 기존 main.cpp에는 주석처리 되어있었으나, 
    // 실제로는 roads에 있는 모델들이 어딘가에서 그려져야 합니다.
    // (만약 roads 벡터를 순회하며 그려야 한다면 아래 코드 사용)
    for (auto& block : mazeBlocks) {
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

    // --- 2. 애니메이션 캐릭터 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
    silverWolf.Draw(shaderProgramAnimated, glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
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
    roads.push_back(new StaticModel("road/road0.obj"));
    roads.push_back(new StaticModel("road/road1.obj"));
    roads.push_back(new StaticModel("road/road2.obj"));
    roads.push_back(new StaticModel("road/road3.obj"));
    roads.push_back(new StaticModel("road/road4.obj"));
    roads.push_back(new StaticModel("road/road5.obj"));
    roads.push_back(new StaticModel("road/road6.obj"));
    roads.push_back(new StaticModel("road/road7.obj"));
    roads.push_back(new StaticModel("road/road8.obj"));
    roads.push_back(new StaticModel("road/road9.obj"));
    roads.push_back(new StaticModel("road/road10.obj"));
    roads.push_back(new StaticModel("road/road11.obj"));
    roads.push_back(new StaticModel("road/road12.obj"));
    roads.push_back(new StaticModel("road/road13.obj"));
    roads.push_back(new StaticModel("road/road14.obj"));
    roads.push_back(new StaticModel("road/road15.obj"));

    // 미로 설정
    setMaze();
    initmaze(&roads);

    silverWolf.silverWolfModel[0] = new NewModel("silver_wolf/Idle.fbx");
    silverWolf.silverWolfModel[0]->state = "idle";
    silverWolf.silverWolfModel[1] = new NewModel("silver_wolf/Walk.fbx");
    silverWolf.silverWolfModel[1]->state = "walk";
    silverWolf.silverWolfModel[2] = new NewModel("silver_wolf/Stop Walking.fbx");
    silverWolf.silverWolfModel[2]->state = "stop_walking";
    silverWolf.silverWolfModel[3] = new NewModel("silver_wolf/Running.fbx");
    silverWolf.silverWolfModel[3]->state = "running";
    silverWolf.silverWolfModel[4] = new NewModel("silver_wolf/Run To Stop.fbx");
    silverWolf.silverWolfModel[4]->state = "run_to_stop";
    silverWolf.silverWolfModel[5] = new NewModel("silver_wolf/Throw.fbx");
    silverWolf.silverWolfModel[5]->state = "throw";
    silverWolf.silverWolfModel[6] = new NewModel("silver_wolf/Stand To Roll.fbx");
    silverWolf.silverWolfModel[6]->state = "stand_to_roll";
}

void game_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(camPos));

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