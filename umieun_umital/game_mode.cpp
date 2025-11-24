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
    for (auto& block : mazeBlocks) {
        update_world_obb(block);
    }

    // 미로 설정
    //setMaze();
    //initmaze(&roads);

    // 카메라 위치
    camPos = glm::vec3(start_x_pos, 5.0f, start_z_pos - 5.0f);
    camTarget = glm::vec3(start_x_pos, 0.0f, start_z_pos+5.0f);

    glEnable(GL_DEPTH_TEST);
}

// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void game_mode::Update(float deltaTime) {
	silverWolf.Update(deltaTime);
    
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

    //glDisable(GL_DEPTH_TEST); // OBB가 겹쳐도 항상 보이게 (선택 사항)
    glLineWidth(3.0f);        // 선 굵기 설정

    for (auto& block : mazeBlocks) {
        if (block.modelPtr) {
            drawDebugOBB(shaderProgramStatic, block.road_world_obb, view, proj, glm::vec3(0.0f, 1.0f, 0.0f));

            for (int i = 0; i < block.obstacle_world_obb.size(); i++)
                drawDebugOBB(shaderProgramStatic, block.obstacle_world_obb[i], view, proj, glm::vec3(1.0f, 1.0f, 0.0f)); // 노란색
        }
    }

    //glEnable(GL_DEPTH_TEST); // DEPTH TEST 복원

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
        roads[i]->set_obb(i);
	}

    // 미로 설정
    setMaze();
    initmaze(&roads);

    silverWolf.Init();

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
    silverWolf.silverWolfModel[7] = new NewModel("silver_wolf/Jump.fbx");
    silverWolf.silverWolfModel[7]->state = "jump";
    silverWolf.silverWolfModel[8] = new NewModel("silver_wolf/Running Jump.fbx");
    silverWolf.silverWolfModel[8]->state = "run_jump";

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

void game_mode::drawDebugOBB(GLuint shaderID, const OBB & obb, const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & color) {
    if (shaderID == 0) return;

    glUseProgram(shaderID);

    // 1. 필수 공통 유니폼 설정 (이동, 카메라 위치, 조명 위치 등)
    setCommonUniforms(shaderID, view, proj);

    GLint originalBUseTexture;
    glm::vec3 originalMaterialColorDefault;
    float originalAmbientStrength;
    int originalShininess;
    glm::vec3 originalMaterialSpecular;

    // A. 현재 상태 저장 (이 값들을 복원해야 함)
    glGetUniformiv(shaderID, glGetUniformLocation(shaderID, "bUseTexture"), &originalBUseTexture);
    glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "materialColorDefault"), glm::value_ptr(originalMaterialColorDefault));
    glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "ambientStrength"), &originalAmbientStrength);
    glGetUniformiv(shaderID, glGetUniformLocation(shaderID, "shininess"), &originalShininess);
    glGetUniformfv(shaderID, glGetUniformLocation(shaderID, "materialSpecular"), glm::value_ptr(originalMaterialSpecular));

    // B. 조명 무시를 위한 값 강제 설정
    glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), 0); // 텍스처 비활성화 [cite: 2, 5]
    glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(color)); // 기본 색상으로 디버그 색상 주입 [cite: 2, 7]
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), 1.0f); // 앰비언트 최대화 (조명 색상 * 기본 색상) [cite: 3, 7]
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), 1); // pow(..., 1)이 되어 specular가 조명 강도에만 영향을 받게 하거나, 
    // 0으로 설정하여 아예 거울 반사를 없앨 수 있습니다.
    // 여기서는 1로 두어 스페큘러 항을 최소화하고, materialSpecular를 0으로 설정합니다. [cite: 3, 11]
    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(glm::vec3(0.0f))); // 스페큘러 기여도 0으로 설정 [cite: 3, 12]

    // ----------------------------------------------------
    // OBB 속성을 이용한 8개 정점 직접 계산
    glm::vec3 E_x = obb.u[0] * obb.half_length.x; // X축 길이 * 방향
    glm::vec3 E_y = obb.u[1] * obb.half_length.y; // Y축 길이 * 방향
    glm::vec3 E_z = obb.u[2] * obb.half_length.z; // Z축 길이 * 방향

    // OBB의 8개 꼭짓점을 월드 좌표로 계산하여 저장
    glm::vec3 vertices[] = {
        // [1] Z+ 면 (앞면)
        obb.center + E_x + E_y + E_z,  // 0: +X +Y +Z
        obb.center - E_x + E_y + E_z,  // 1: -X +Y +Z
        obb.center + E_x - E_y + E_z,  // 2: +X -Y +Z
        obb.center - E_x - E_y + E_z,  // 3: -X -Y +Z

        // [2] Z- 면 (뒷면)
        obb.center + E_x + E_y - E_z,  // 4: +X +Y -Z
        obb.center - E_x + E_y - E_z,  // 5: -X +Y -Z
        obb.center + E_x - E_y - E_z,  // 6: +X -Y -Z
        obb.center - E_x - E_y - E_z   // 7: -X -Y -Z
    };

    // 12개의 선분을 그리기 위한 24개의 인덱스 (순서는 유지)
    unsigned int indices[] = {
        1, 0, 0, 2, 2, 3, 3, 1, // 앞면 (Z+)
        5, 4, 4, 6, 6, 7, 7, 5, // 뒷면 (Z-)
        1, 5, 0, 4, 2, 6, 3, 7  // 기둥
    };

    GLuint obbVAO, obbVBO, obbEBO;
    glGenVertexArrays(1, &obbVAO);
    glGenBuffers(1, &obbVBO);
    glGenBuffers(1, &obbEBO);

    // --- VAO 설정 ---
    glBindVertexArray(obbVAO);

    glBindBuffer(GL_ARRAY_BUFFER, obbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obbEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 정점 속성 포인터 설정 (기존 셰이더의 aPos 위치 0 사용)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1); // aNormal (Location 1) 
    glDisableVertexAttribArray(2); // aTexCoords (Location 2)

    // ----------------------------------------------------
    // Model Matrix 구성 및 드로우

    // OBB의 정보를 기반으로 Model Matrix를 구성 (Center + Orientation + Scale)
    glm::mat4 model = glm::mat4(1.0f);

    //model[0] = glm::vec4(obb.u[0] * obb.half_length.x, 0.0f); // X축 (u[0])
    //model[1] = glm::vec4(obb.u[1] * obb.half_length.y, 0.0f); // Y축 (u[1])
    //model[2] = glm::vec4(obb.u[2] * obb.half_length.z, 0.0f); // Z축 (u[2])
    //model[3] = glm::vec4(obb.center, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    // 드로우 호출 (GL_LINES)
    glBindVertexArray(obbVAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // --- 리소스 해제 ---
    glDeleteBuffers(1, &obbVBO);
    glDeleteBuffers(1, &obbEBO);
    glDeleteVertexArrays(1, &obbVAO);

    // 환경 복원
    glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), originalBUseTexture);
    glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(originalMaterialColorDefault));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), originalAmbientStrength);
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), originalShininess);
    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(originalMaterialSpecular));
}

void game_mode::Keyboard(unsigned char key, int x, int y) {

    silverWolf.Keyboard(key, x, y);

}
void game_mode::Keyupboard(unsigned char key, int x, int y) {

    silverWolf.Keyupboard(key, x, y);
}
