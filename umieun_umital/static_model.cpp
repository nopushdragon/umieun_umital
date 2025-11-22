#include "static_model.h"

// ===================================================================
// StaticMesh
// ===================================================================
void StaticMesh::setupMesh() {
    
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

void StaticMesh::Draw(GLuint shaderID) const {
    // 텍스처 바인딩 로직
    if (!textures.empty() && textures[0].id != 0) {
        // 텍스처가 로드되었으면 텍스처 유닛 0을 활성화하고 바인딩
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0].id);
        // 셰이더에게 텍스처 유닛 0을 사용한다고 알림
        glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), true); // 텍스처 사용 플래그
    }
    else {
        // 텍스처가 없으면 텍스처 사용 플래그를 끄고 기본 색상을 사용
        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), false);
        glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(textures[0].diffuseColor));
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ===================================================================
// StaticModel
// ===================================================================
StaticModel::StaticModel(const std::string& objPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // 텍스처/재질 로딩은 OBJ 파일의 경로에 따라 다름 (구현 필요)

    processNode(scene->mRootNode, scene);
}

void StaticModel::Draw(GLuint shaderID) {
    for (auto& mesh : meshes) mesh.Draw(shaderID);
}

void StaticModel::processNode(aiNode* node, const aiScene* scene)
{
    // 현재 노드가 가진 모든 메시를 처리합니다.
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // 자식 노드를 재귀적으로 순회하며 처리합니다.
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

StaticMesh StaticModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // 텍스처 로딩 구현 필요

    // 1. 정점 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        StaticVertex vertex;
        // 위치 (Position)
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // 법선 (Normal)
        if (mesh->mNormals)
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        // 텍스처 좌표 (TexCoords)
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // 2. 인덱스 데이터 (Faces) 추출
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 3. 재질/텍스처 처리 (구현)
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        Texture matInfo;

        // 3-1. 확산 색상 (Kd) 추출 및 저장
        aiColor4D color(0.f, 0.f, 0.f, 1.f);
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color))
        {
            matInfo.diffuseColor = glm::vec3(color.r, color.g, color.b);
        }

        // 3-2. 텍스처 파일 경로 추출 및 로딩
        aiString str;
        // AI_MATKEY_TEXTURE_DIFFUSE: MTL 파일의 map_Kd (확산 텍스처) 경로를 찾습니다.
        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &str))
        {
            // 경로가 있다면 텍스처 로딩 시도
            matInfo.path = str.C_Str();
            // 텍스처 로드 함수 호출 (현재 파일 경로를 기준으로 로드)
            // OBJ 파일이 있는 "현재 디렉토리"를 가정하고 경로 전달
            matInfo.id = loadTextureFromFile(matInfo.path.c_str(), ".");
            matInfo.type = "texture_diffuse";
        }
        else {
            // 텍스처 경로가 없는 경우, ID를 0으로 설정하여 텍스처를 사용하지 않도록 표시
            matInfo.id = 0;
            matInfo.type = "color_diffuse";
        }

        // 재질 정보를 메시의 첫 번째 텍스처 항목에 저장
        textures.push_back(matInfo);
    }

    // 메시 객체 생성 및 설정
    StaticMesh staticMesh;
    staticMesh.vertices = vertices;
    staticMesh.indices = indices;
    staticMesh.textures = textures;
    staticMesh.setupMesh(); // VAO/VBO/EBO 설정 호출

    return staticMesh;
}