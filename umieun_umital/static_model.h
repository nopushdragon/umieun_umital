#include "headers.h"

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

public:
    std::vector<StaticMesh> meshes;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    StaticModel(const std::string& objPath);

    void Draw(GLuint shaderID);
};
