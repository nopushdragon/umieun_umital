#define STB_IMAGE_IMPLEMENTATION
#include "headers.h"
#include "animated_model.h"
#include "static_model.h"
#include "maze.h"

GLuint shaderProgramStatic; // 정적 모델 셰이더 프로그램 ID
GLuint shaderProgramAnimated; // 애니메이션 모델 셰이더 프로그램 ID
const char* STATIC_VERT = "static_vertex.glsl";
const char* ANIMATED_VERT = "animated_vertex.glsl";
const char* FRAGMENT_LIGHT = "fragment.glsl";

// ===================================================================
// 3. 씬 관리 및 메인 함수
// ===================================================================

// --- 미로 배치 인스턴스 ---
struct MazeBlockInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
    glm::vec3 reset;    // 미로 초기 위치 저장용
};

// 전역 씬 객체
AnimatedModel* playerModel = nullptr;
StaticModel* wallModel = nullptr;
std::vector<MazeBlockInstance> mazeBlocks;

// 카메라 위치 (예시)
glm::vec3 camPos(0.0f, 30.0f, 30.0f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// 광원/재질 정보 (셰이더 유니폼으로 전송)
glm::vec3 lightPos(10.0f, 20.0f, 10.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 materialSpecular(1.0f, 1.0f, 1.0f);
float ambientStrength = 0.1f;
int shininess = 32;

//
std::vector<StaticModel*> roads;
//

// --- 셰이더 유틸리티 함수 ---
char* filetobuf(const char* file); // 파일 내용을 문자열로 읽어오는 함수
char* filetobuf(const char* file) {
    ifstream f(file, ios::binary);
    if (!f.is_open()) {
        cerr << "ERROR: Cannot open shader file: " << file << endl;
        return nullptr;
    }
    f.seekg(0, ios::end);
    int len = f.tellg();
    char* buf = new char[len + 1];
    f.seekg(0, ios::beg);
    f.read(buf, len);
    buf[len] = '\0';
    f.close();
    return buf;
}

void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID);
void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
    GLuint vertShader, fragShader;
    GLint success;
    GLchar infoLog[512];

    // 1. 버텍스 셰이더 컴파일
    const char* vertCode = filetobuf(vertPath);
    if (!vertCode) return;
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCode, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << vertPath << "\n" << infoLog << endl;
    }
    delete[] vertCode;

    // 2. 프래그먼트 셰이더 컴파일
    const char* fragCode = filetobuf(fragPath);
    if (!fragCode) { glDeleteShader(vertShader); return; }
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << fragPath << "\n" << infoLog << endl;
    }
    delete[] fragCode;

    // 3. 셰이더 프로그램 링크
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertShader);
    glAttachShader(shaderID, fragShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    // 셰이더 객체는 프로그램에 링크된 후 삭제 가능
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void loadModels() {
    roads.push_back(new StaticModel("road0.obj"));  // 0동
    roads.push_back(new StaticModel("road0.obj"));  // 1서 
	roads.push_back(new StaticModel("road0.obj"));  // 2남 
	roads.push_back(new StaticModel("road0.obj"));  // 3북
	roads.push_back(new StaticModel("road0.obj"));  // 4ㅡ
	roads.push_back(new StaticModel("road0.obj"));  // 5ㅣ
	roads.push_back(new StaticModel("road0.obj"));  // 6┌
	roads.push_back(new StaticModel("road0.obj"));  // 7┐
	roads.push_back(new StaticModel("road0.obj"));  // 8└
	roads.push_back(new StaticModel("road0.obj"));  // 9┘
	roads.push_back(new StaticModel("road0.obj"));  // 10ㅏ
	roads.push_back(new StaticModel("road0.obj"));  // 11ㅓ
	roads.push_back(new StaticModel("road0.obj"));  // 12ㅜ
	roads.push_back(new StaticModel("road0.obj"));  // 13ㅗ
	roads.push_back(new StaticModel("road0.obj"));  // 14+
    roads.push_back(new StaticModel("road0.obj"));  // 15x
}

void init() {
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glewExperimental = GL_TRUE;
    glewInit();

    // 셰이더 로드
    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);

    loadModels();

    // road 인스턴스를 씬 중앙에 배치
    MazeBlockInstance roadInstance;
    roadInstance.modelPtr = road;
    roadInstance.modelMatrix = glm::mat4(1.0f); // Model 행렬 초기화 (월드 원점)
    roadInstance.reset = glm::vec3(1.0f);
    mazeBlocks.push_back(roadInstance);
    //

    // 미로 생성 및 StaticModel 인스턴스 배치 로직 (구현 필요)
    // 예시: mazeBlocks.push_back({wallModel, glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))});
}

void setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(camPos));

    // 조명 유니폼
    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);

    // 재질 유니폼 (Diffuse는 셰이더에서 텍스처 샘플링으로 대체됨)
    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
}

void drawScene() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 view = glm::lookAt(camPos, camTarget, camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    // --- 1. 정적 오브젝트 렌더링 (미로 벽, 바닥) ---
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    //
    glm::vec3 exampleSpecular = glm::vec3(0.35f, 0.35f, 0.35f); // Ks 
    int exampleShininess = 32;                                   // Ns 
    // 재질 유니폼 설정
    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "materialSpecular"), 1, glm::value_ptr(exampleSpecular));
    glUniform1i(glGetUniformLocation(shaderProgramStatic, "shininess"), exampleShininess);
    //

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

    // --- 2. 애니메이션 캐릭터 렌더링 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    if (playerModel) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramAnimated, "uModel"), 1, GL_FALSE, glm::value_ptr(playerModel->modelMatrix));
        playerModel->Draw(shaderProgramAnimated);
    }

    glutSwapBuffers();
}

void timer(int value) {
    // 플레이어 입력 및 충돌 감지 로직 (구현 필요)

    if (playerModel) {
        // 애니메이션 업데이트 (시간 기반)
        // playerModel->updateAnimation(glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
    }

    //model 업데이트
    for (auto& block : mazeBlocks) {
        block.modelMatrix = glm::mat4(1.0f);
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("3D Maze Framework");

    setMaze();
    
    init();

    glutDisplayFunc(drawScene);
    glutTimerFunc(1000 / 60, timer, 1);
    glutMainLoop();
    return 0;
}