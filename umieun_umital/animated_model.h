#pragma once
#include "headers.h"

using namespace std;

// 한 정점이 영향을 받는 최대 뼈 개수
#define MAX_BONE_INFLUENCE 4

// -----------------------------------------------------------
// 구조체 정의
// -----------------------------------------------------------
struct ModelVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture_fbx {
    GLuint id;
    string type;
    string path;
};

struct BoneInfo {
    int id;
    glm::mat4 offset; // Bind Pose
    glm::mat4 finalTransformation;
};

// -----------------------------------------------------------
// AssimpGLMHelpers 클래스 (선언)
// -----------------------------------------------------------
class AssimpGLMHelpers {
public:
    static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
    static glm::vec3 GetGLMVec(const aiVector3D& vec);
    static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};

// -----------------------------------------------------------
// Mesh 클래스 (선언)
// -----------------------------------------------------------
class Mesh {
public:
    vector<ModelVertex> vertices;
    vector<unsigned int> indices;
    vector<Texture_fbx> textures;
    unsigned int VAO;

    Mesh(vector<ModelVertex> vertices, vector<unsigned int> indices, vector<Texture_fbx> textures);
    void Draw(GLuint shaderID);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};

// -----------------------------------------------------------
// NewModel 클래스 (선언)
// -----------------------------------------------------------
class NewModel {
public:
    vector<Texture_fbx> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    // 모델 Transform
    glm::vec3 pos= glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float angle = 0.0f;

    string state = "none";

    // 애니메이션 데이터
    map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter;
    Assimp::Importer m_Importer;
    const aiScene* m_Scene;
    glm::mat4 m_GlobalInverseTransform;

    int currentTextureNum = 1;

    NewModel(string const& path, bool gamma = false);
    void Draw(GLuint shaderID, float currentTime);

    // 외부 호출용 뼈 변형 함수
    void BoneTransform(float timeInSeconds, vector<glm::mat4>& Transforms);

    ~NewModel();

private:
    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture_fbx> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char* path);

    // 뼈 관련 헬퍼
    void SetVertexBoneDataToDefault(ModelVertex& vertex);
    void SetVertexBoneData(ModelVertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(vector<ModelVertex>& vertices, aiMesh* mesh, const aiScene* scene);

    // 애니메이션 보간 헬퍼
    glm::mat4 CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    glm::mat4 CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    glm::mat4 CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    void RemoveRootMotion();
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& parentTransform);
};