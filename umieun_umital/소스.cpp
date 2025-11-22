#include "headers.h"

// ===================================================================
// 상수 및 전역 변수 정의
// ===================================================================
#define MAX_BONE_PER_VERTEX 4
#define MAX_BONES           100
#define WINDOW_WIDTH        1200
#define WINDOW_HEIGHT       800

GLuint shaderProgramStatic; // 정적 모델 셰이더 프로그램 ID
GLuint shaderProgramAnimated; // 애니메이션 모델 셰이더 프로그램 ID
// 셰이더 파일 경로 (예시)
const char* STATIC_VERT = "static_model.vert";
const char* ANIMATED_VERT = "animated_model.vert";
const char* FRAGMENT_LIGHT = "fragment_light.glsl";

// --- 셰이더 유틸리티 함수 ---
char* filetobuf(const char* file); // 파일 내용을 문자열로 읽어오는 함수
void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID);

// --- 뼈대 정보 구조체 (FBX 애니메이션용) ---
struct BoneInfo {
    int id;
    glm::mat4 offset; // Assimp의 오프셋 행렬을 GLM 행렬로 변환해야 함
};
void aiMatrix4x4ToGlm(const aiMatrix4x4* from, glm::mat4& to); // Assimp 행렬을 GLM으로 변환하는 함수 (구현 필요)

// --- 텍스처 정보 구조체 (MTL/FBX 재질용) ---
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
    // 텍스처 파일 로딩 함수 (구현 필요)
};
//으하하 해킹함 ㅇㅅㅇ
// ===================================================================
// 1. 🧊 정적 오브젝트 구조 (StaticModel)
// ===================================================================

struct StaticVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct StaticMesh {
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO, VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 위치 속성 (layout=0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)0);
        // 법선 속성 (layout=1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, Normal));
        // 텍스처 좌표 속성 (layout=2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, TexCoords));

        glBindVertexArray(0);
    }

    void Draw(GLuint shaderID) {
        // 텍스처 바인딩 로직: textures 벡터를 순회하며 glActiveTexture, glBindTexture 호출
        // ... (구현 필요)

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

class StaticModel {
private:
    void processNode(aiNode* node, const aiScene* scene);
    StaticMesh processMesh(aiMesh* mesh, const aiScene* scene);
    // 텍스처 로딩 유틸리티 (구현 필요)

public:
    std::vector<StaticMesh> meshes;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    StaticModel(const std::string& objPath) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(objPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // 텍스처/재질 로딩은 OBJ 파일의 경로에 따라 다름 (구현 필요)

        processNode(scene->mRootNode, scene);
    }

    void Draw(GLuint shaderID) {
        for (auto& mesh : meshes) mesh.Draw(shaderID);
    }
};

// ===================================================================
// 2. 🚶‍♂️ 애니메이션 캐릭터 구조 (AnimatedModel)
// ===================================================================

struct AnimatedVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    int BoneIDs[MAX_BONE_PER_VERTEX];
    float Weights[MAX_BONE_PER_VERTEX];
};

struct AnimatedMesh {
    std::vector<AnimatedVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO, VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AnimatedVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 위치(0), 법선(1), 텍스처 좌표(2) 설정 (StaticVertex와 동일)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, TexCoords));

        // 뼈대 인덱스 속성 (layout=3, 정수 벡터)
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, BoneIDs));

        // 가중치 속성 (layout=4, 실수 벡터)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)offsetof(AnimatedVertex, Weights));

        glBindVertexArray(0);
    }

    void Draw(GLuint shaderID) {
        // 텍스처 바인딩 로직: textures 벡터를 순회하며 glActiveTexture, glBindTexture 호출
        // ... (구현 필요)

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

class AnimatedModel {
private:
    void processNode(aiNode* node, const aiScene* scene);
    AnimatedMesh processMesh(aiMesh* mesh, const aiScene* scene);
    // 뼈대 정보 로딩 함수 (FBX 파싱 시 호출)
    void extractBoneWeight(AnimatedMesh& mesh, aiMesh* aiMesh);
    // 텍스처 로딩 유틸리티 (구현 필요)

public:
    std::vector<AnimatedMesh> meshes;
    std::map<std::string, BoneInfo> boneInfoMap; // 뼈 이름과 정보 매핑
    std::vector<glm::mat4> finalBoneMatrices; // 매 프레임 계산된 뼈대 행렬
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    int boneCounter = 0;

    AnimatedModel(const std::string& fbxPath) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(fbxPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // 애니메이션 관련 초기화 로직: 뼈대 계층 구조 파싱 및 finalBoneMatrices 벡터 크기 조정
        // ... (구현 필요)
        finalBoneMatrices.resize(MAX_BONES, glm::mat4(1.0f));

        processNode(scene->mRootNode, scene);
    }

    void updateAnimation(float time) {
        // 애니메이션 재생 로직: 시간(time)을 기준으로 뼈 행렬(finalBoneMatrices)을 계산
        // Assimp의 애니메이션, 노드 변환, 키프레임 보간 로직 포함
        // ... (매우 복잡, 구현 필요)
    }

    void Draw(GLuint shaderID) {
        // 최종 뼈대 행렬을 셰이더 유니폼으로 전송
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uFinalBoneMatrices"),
            finalBoneMatrices.size(), GL_FALSE, glm::value_ptr(finalBoneMatrices[0]));

        for (auto& mesh : meshes) mesh.Draw(shaderID);
    }
};

// --- 미로 배치 인스턴스 ---
struct MazeBlockInstance {
    StaticModel* modelPtr;
    glm::mat4 modelMatrix;
};

// ===================================================================
// 3. 씬 관리 및 메인 함수
// ===================================================================

// 전역 씬 객체
AnimatedModel* playerModel = nullptr;
StaticModel* wallModel = nullptr;
std::vector<MazeBlockInstance> mazeBlocks;

// 카메라 위치 (예시)
glm::vec3 camPos(0.0f, 10.0f, 20.0f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// 광원/재질 정보 (셰이더 유니폼으로 전송)
glm::vec3 lightPos(10.0f, 20.0f, 10.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 materialSpecular(1.0f, 1.0f, 1.0f);
float ambientStrength = 0.1f;
int shininess = 32;

//
StaticModel* tankModel = nullptr;
//

void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
    // 셰이더 파일 읽기, 컴파일, 링크 로직
    // ... (기존 코드의 make_vertexShaders, make_fragmentShaders, make_shaderProgram과 유사)
    // ... (구현 필요)
}

void init() {
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glewExperimental = GL_TRUE;
    glewInit();

    // 셰이더 로드
    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);

    // 3D 모델 로드 (파일 경로 예시)
    // wallModel = new StaticModel("assets/wall_cube.obj");
    // playerModel = new AnimatedModel("assets/player.fbx");
    
    //
    tankModel = new StaticModel("n_tank.obj");
    // 탱크 인스턴스를 씬 중앙에 배치
    MazeBlockInstance tankInstance;
    tankInstance.modelPtr = tankModel;
    tankInstance.modelMatrix = glm::mat4(1.0f); // Model 행렬 초기화 (월드 원점)
    mazeBlocks.push_back(tankInstance);
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

    for (const auto& block : mazeBlocks) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(block.modelMatrix));
        if (block.modelPtr) block.modelPtr->Draw(shaderProgramStatic);
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

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("3D Maze Framework");

    init();

    glutDisplayFunc(drawScene);
    glutTimerFunc(1000 / 60, timer, 1);
    glutMainLoop();
    return 0;
}