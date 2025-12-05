#include "static_model.h"
// ===================================================================
// StaticMesh
// ===================================================================
void StaticMesh::setupMesh() {
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

void StaticMesh::Draw(GLuint shaderID) const {
    const Texture& meshTexture = textures[0];

    if (!textures.empty() && meshTexture.id != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshTexture.id);
        glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), true);
    }
    else {
        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), false);
        glm::vec3 diffuseColorToUse = meshTexture.diffuseColor;

        if (glm::length(diffuseColorToUse) < 0.1f) {
            diffuseColorToUse = glm::vec3(0.5f, 0.5f, 0.5f);
        }

        glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(diffuseColorToUse));
    }

    // textures가 비어있어도 무조건 설정 (전역값 덮어쓰기)
    if (!textures.empty()) {
        glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(meshTexture.specularColor));
        glUniform1i(glGetUniformLocation(shaderID, "shininess"), meshTexture.shininess);
    }
    else {
        // textures가 비어있으면 specular를 0으로 강제 설정
        glm::vec3 zeroSpecular(0.0f, 0.0f, 0.0f);
        glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(zeroSpecular));
        glUniform1i(glGetUniformLocation(shaderID, "shininess"), 1);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ===================================================================
// StaticModel
// ===================================================================
StaticModel::StaticModel(const std::string& objPath) {
    // 1. 디렉토리 설정 (매우 중요)
    directory = objPath.substr(0, objPath.find_last_of('/'));
    if (directory == objPath) directory = "."; // 슬래시가 없으면 현재 경로

    // 2. 바이너리 파일 확인 및 로딩 시도
    std::string binPath = objPath.substr(0, objPath.find_last_of('.')) + ".bin";

    // 디버깅용 바이너리 파일 삭제 강제
    //std::remove(binPath.c_str());

    //if (LoadFromBinary(binPath)) {
    //    return; // 성공 시 Assimp 스킵
    //}

    // 3. 실패 시 Assimp로 로딩
    Assimp::Importer importer;
    // Tangent가 필요 없다면 CalcTangentSpace 옵션 빼도 됨 (속도 향상)
    const aiScene* scene = importer.ReadFile(objPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace| aiProcess_GenSmoothNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);

    // 4. 로딩 완료 후 저장 (다음 실행을 위해)
    SaveToBinary(binPath);
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
    std::vector<Texture> textures;

    // 1. 정점 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        StaticVertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->mNormals)
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // 2. 인덱스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 3. 재질/텍스처 처리
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        Texture matInfo;

        // Diffuse Color (Kd) 추출
        aiColor4D color_d(1.f, 1.f, 1.f, 1.f);
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color_d)) {
            matInfo.diffuseColor = glm::vec3(color_d.r, color_d.g, color_d.b);

            // ✅ 모든 road의 밝기를 0.18로 균일하게 강제 (MTL 기본값과 동일)
            if (this->directory.find("road") != std::string::npos) {
                matInfo.diffuseColor = glm::vec3(0.1808f, 0.1808f, 0.1808f);
            }
        }

        // Specular Color (Ks) 추출
        aiColor4D color_s(0.f, 0.f, 0.f, 1.f);
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color_s)) {
            matInfo.specularColor = glm::vec3(color_s.r, color_s.g, color_s.b);
        }

        float shininess_val = 1.0f;
        if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess_val)) {
            matInfo.shininess = (int)shininess_val;
        }

        if (glm::length(matInfo.specularColor) > 1.0f) {
            matInfo.specularColor *= 0.05f;
        }

        // 텍스처 파일 경로 추출
        aiString str;
        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &str))
        {
            matInfo.path = str.C_Str();
            matInfo.id = loadTextureFromFile(matInfo.path.c_str(), this->directory);
            matInfo.type = "texture_diffuse";
        }
        else {
            matInfo.id = 0;
            matInfo.type = "color_diffuse";
        }

        textures.push_back(matInfo);
    }

    StaticMesh staticMesh;
    staticMesh.vertices = vertices;
    staticMesh.indices = indices;
    staticMesh.textures = textures;
    staticMesh.setupMesh();

    return staticMesh;
}

void StaticModel::set_maze_obb(int idx) {
    road_local_obb.center = glm::vec3(0.0f, 0.0f, 0.0f);;
    road_local_obb.half_length = glm::vec3(ROAD_SIZE / 2, 0.1f, ROAD_SIZE / 2);
    road_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    road_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    road_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);

    if (idx == 0) { // 동
        maze_obb_block(true, true, true, true, false, false, true, true, true);
    }
    else if (idx == 1) { // 서
        maze_obb_block(true, true, true, false, false, true, true, true, true);
    }
    else if (idx == 2) { // 남
        maze_obb_block(true, true, true, true, false, true, true, false, true);
    }
    else if (idx == 3) { // 북
        maze_obb_block(true, false, true, true, false, true, true, true, true);
    }
    else if (idx == 4) { // ㅡ
        maze_obb_block(true, true, true, false, false, false, true, true, true);
    }
    else if (idx == 5) { // ㅣ
        maze_obb_block(true, false, true, true, false, true, true, false, true);
	}
    else if (idx == 6) { // ┌
        maze_obb_block(true, true, true, true, false, false, true, false, true);
    }
    else if (idx == 7) { // ┐
        maze_obb_block(true, true, true, false, false, true, true, false, true);
    }
    else if (idx == 8) { // └
        maze_obb_block(true, false, true, true, false, false, true, true, true);
    }
    else if (idx == 9) { // ┘
        maze_obb_block(true, false, true, false, false, true, true, true, true);
    }
    else if (idx == 10) { // ㅏ
        maze_obb_block(true, false, true, true, false, false, true, false, true);
    }
    else if (idx == 11) { // ㅓ
        maze_obb_block(true, false, true, false, false, true, true, false, true);
    }
    else if (idx == 12) { // ㅜ
        maze_obb_block(true, true, true, false, false, false, true, false, true);
    }
    else if (idx == 13) { // ㅗ
        maze_obb_block(true, false, true, false, false, false, true, true, true);
    }
    else if (idx == 14) { // +
        maze_obb_block(true, false, true, false, false, false, true, false, true);
    }
    else if (idx == 15) { // x
        maze_obb_block(true, true, true, true, true, true, true, true, true);
	}
}

void StaticModel::maze_obb_block(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h, bool i) {
    bool corners[9] = { a, b, c, d, e, f, g, h, i };
    int corner_idx = 0;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (corners[corner_idx]) {
                OBB obstacle;
                float obb_half_size = ROAD_SIZE / 6;
                obstacle.center = glm::vec3((j * obb_half_size) * 2, ROAD_SIZE / 2, (i * obb_half_size) * 2);
                obstacle.half_length = glm::vec3(obb_half_size, ROAD_SIZE / 2, obb_half_size);
                obstacle.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
                obstacle.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
                obstacle.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
                obstacle_local_obb.push_back(obstacle);
            }
            corner_idx++;
        }
    }
}

void StaticModel::set_target_obb(){
    target_local_obb.center = glm::vec3(0.0f, 0.0f, 0.0f);;
    target_local_obb.half_length = glm::vec3(target_size / 2, target_size / 2, target_size / 2);
    target_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    target_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    target_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}

void StaticModel::set_chest_obb() {
    chest_local_obb.center = glm::vec3(0.0f, 0.0f, 0.0f);;
    chest_local_obb.half_length = glm::vec3(chest_size, chest_size, chest_size/2);
    chest_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    chest_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    chest_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}
void StaticModel::set_ball_obb() {
    ball_local_obb.center = glm::vec3(0.0f, 0.0f, 0.0f);;
    ball_local_obb.half_length = glm::vec3(ball_size, ball_size, ball_size);
    ball_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    ball_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    ball_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
}

void StaticModel::SaveToBinary(const std::string& fileName) {
    std::ofstream out(fileName, std::ios::out | std::ios::binary);
    if (!out.is_open()) return;

    // 1. 메쉬 개수
    int meshCount = meshes.size();
    out.write((char*)&meshCount, sizeof(int));

    // 2. 메쉬 데이터
    for (const auto& mesh : meshes) {
        // 정점
        int vSize = mesh.vertices.size();
        out.write((char*)&vSize, sizeof(int));
        out.write((char*)mesh.vertices.data(), vSize * sizeof(StaticVertex));

        // 인덱스
        int iSize = mesh.indices.size();
        out.write((char*)&iSize, sizeof(int));
        out.write((char*)mesh.indices.data(), iSize * sizeof(unsigned int));

        // 텍스처 & 재질 정보
        int texSize = mesh.textures.size();
        out.write((char*)&texSize, sizeof(int));

        for (const auto& tex : mesh.textures) {
            // A. 문자열 정보 (Type, Path)
            std::string type = tex.type;
            int typeLen = type.length();
            out.write((char*)&typeLen, sizeof(int));
            out.write(type.c_str(), typeLen);

            std::string path = tex.path;
            int pathLen = path.length();
            out.write((char*)&pathLen, sizeof(int));
            out.write(path.c_str(), pathLen);

            // ★ [추가] B. 재질 색상 정보 (Diffuse, Specular, Shininess)
            // OBJ는 텍스처 없이 색상만 있는 경우가 많으므로 필수 저장
            out.write((char*)&tex.diffuseColor, sizeof(glm::vec3));
            out.write((char*)&tex.specularColor, sizeof(glm::vec3));
            out.write((char*)&tex.shininess, sizeof(int));
        }
    }

    out.close();
    std::cout << "바이너리 저장 완료 (Static): " << fileName << std::endl;
}

bool StaticModel::LoadFromBinary(const std::string& fileName) {
    std::ifstream in(fileName, std::ios::in | std::ios::binary);
    if (!in.is_open()) return false;

    int meshCount = 0;
    in.read((char*)&meshCount, sizeof(int));

    meshes.clear();
    meshes.reserve(meshCount);

    for (int i = 0; i < meshCount; i++) {
        std::vector<StaticVertex> _vertices;
        std::vector<unsigned int> _indices;
        std::vector<Texture> _textures;

        // 정점
        int vSize = 0;
        in.read((char*)&vSize, sizeof(int));
        _vertices.resize(vSize);
        in.read((char*)_vertices.data(), vSize * sizeof(StaticVertex));

        // 인덱스
        int iSize = 0;
        in.read((char*)&iSize, sizeof(int));
        _indices.resize(iSize);
        in.read((char*)_indices.data(), iSize * sizeof(unsigned int));

        // 텍스처 & 재질
        int texSize = 0;
        in.read((char*)&texSize, sizeof(int));

        for (int k = 0; k < texSize; k++) {
            Texture tex;

            // A. 문자열 읽기
            int typeLen = 0;
            in.read((char*)&typeLen, sizeof(int));
            std::string type(typeLen, '\0');
            in.read(&type[0], typeLen);
            tex.type = type;

            int pathLen = 0;
            in.read((char*)&pathLen, sizeof(int));
            std::string path(pathLen, '\0');
            in.read(&path[0], pathLen);
            tex.path = path;

            // ★ [추가] B. 재질 색상 읽기
            in.read((char*)&tex.diffuseColor, sizeof(glm::vec3));
            in.read((char*)&tex.specularColor, sizeof(glm::vec3));
            in.read((char*)&tex.shininess, sizeof(int));

            // C. 텍스처 파일 로딩 (경로가 있을 때만)
            if (!tex.path.empty()) {
                std::string fullPath = this->directory + '/' + tex.path;
                tex.id = loadTextureFromFile(tex.path.c_str(), this->directory); // 인자 1개 버전 사용 시 수정 필요
                // 만약 loadTextureFromFile 인자가 2개라면: loadTextureFromFile(tex.path.c_str(), this->directory);
            }
            else {
                tex.id = 0; // 텍스처 없음
            }

            _textures.push_back(tex);
        }

        StaticMesh mesh;
        mesh.vertices = _vertices;
        mesh.indices = _indices;
        mesh.textures = _textures;
        mesh.setupMesh(); // ★ 필수: VAO/VBO 생성

        meshes.push_back(mesh);
    }

    in.close();
    //std::cout << "바이너리 로드 성공 (Static): " << fileName << std::endl;
    return true;
}