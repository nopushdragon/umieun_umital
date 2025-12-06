#include "clear_mode.h"
#include "game_mode.h"
#include "option_mode.h"
#include "game_framwork.h"
#include "account.h"
#include "title_mode.h"

#define STB_IMAGE_IMPLEMENTATION

// 생성자: 변수 초기값 설정
clear_mode::clear_mode() {
}

clear_mode::~clear_mode() {
    // Finish()가 호출되지 않고 파괴될 경우를 대비
    // (일반적으로는 Finish에서 정리함)
}

void clear_mode::Finish() {
}

void clear_mode::loadModels() {
}
void clear_mode::loadImages() {
}
void clear_mode::loadTexts() {
}

// 1. 초기화 (기존 init 함수 내용)
void clear_mode::Init() {
}

//float title_deltatime;
// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void clear_mode::Update(float deltaTime) {
}

// 3. 그리기 (기존 drawScene 함수 내용)
void clear_mode::Draw() {
}

void clear_mode::Fog_And_Flashlight_Update() {
}

void clear_mode::Keyboard(unsigned char key, int x, int y) {
}

void clear_mode::Keyupboard(unsigned char key, int x, int y) {
}

void clear_mode::SpecialKeyboard(int key, int x, int y) {
}

void clear_mode::SpecialUpKeyboard(int key, int x, int y) {
}

void clear_mode::Mouse(int button, int state, int x, int y) {
}

void clear_mode::PassiveMotion(int x, int y) {
}

void  clear_mode::Motion(int x, int y) {
}

void clear_mode::OnPause() {
    // 옵션 창 등을 열었을 때 멈춰야 할 로직이 있다면 여기에 작성
}

void clear_mode::OnResume() {
    // 옵션 창 닫고 돌아왔을 때 복구할 로직

    glEnable(GL_DEPTH_TEST); // 혹시 다른 씬에서 껐을까봐 다시 켬
}

void clear_mode::Reshape(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    glViewport(0, 0, w, h);
}

void clear_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(gameCamera.camPos));

    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);

    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
}

char* clear_mode::filetobuf(const char* file) {
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

void clear_mode::loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
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