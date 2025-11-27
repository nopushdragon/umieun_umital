#pragma once
#include "headers.h"
#include "texture.h"

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

    void setupMesh();

    void Draw(GLuint shaderID) const;
};

class StaticModel {
private:
    void processNode(aiNode* node, const aiScene* scene);
    StaticMesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::string directory;

public:
    std::vector<StaticMesh> meshes;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // 미로 전용
    OBB road_local_obb;  // 로컬 OBB(땅)
	std::vector<OBB> obstacle_local_obb; // 장애물 로컬 OBB 리스트
	void set_maze_obb(int idx); // OBB 설정 함수 idx에 따라 장애물 다름
	void maze_obb_block(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h, bool i); // 미로 OBB 세팅 함수

    //과녁
	OBB target_local_obb; //로컬 OBB(과녁)
	float target_size = 2.0f;
	void set_target_obb();

    //보물

    //캐시
    void SaveToBinary(const std::string& fileName);
    bool LoadFromBinary(const std::string& fileName);
    StaticModel(const std::string& objPath);

    void Draw(GLuint shaderID);
};
