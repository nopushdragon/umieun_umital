//#include "toy.h"
//
//// ===================================================================
//// StaticMesh
//// ===================================================================
//void StaticMesh::setupMesh() {
//    glGenVertexArrays(1, &VAO);
//
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticVertex), &vertices[0], GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//
//    // 위치 속성 (layout=0)
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)0);
//    // 법선 속성 (layout=1)
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, Normal));
//    // 텍스처 좌표 속성 (layout=2)
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, TexCoords));
//
//    glBindVertexArray(0);
//}
//
//void StaticMesh::Draw(GLuint shaderID) const {
//    const Texture& meshTexture = textures[0];
//
//    // 1. 텍스처/Diffuse 색상 설정
//    if (!textures.empty() && meshTexture.id != 0) {
//        // ... (기존 텍스처 로직 유지)
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, meshTexture.id);
//        glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
//        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), true);
//    }
//    else {
//        // 텍스처 로드 실패 또는 텍스처가 없는 경우:
//        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), false);
//
//        // MTL의 Kd를 사용하되, 너무 어두우면 중간 회색을 기본값으로 사용 (선택적)
//        glm::vec3 diffuseColorToUse = meshTexture.diffuseColor;
//
//        // Kd가 너무 어두워서 흑백으로 보인다면, 기본 회색으로 대체 (디버그용 안전 장치)
//        if (glm::length(diffuseColorToUse) < 0.1f) { // 흑색 Kd (0,0,0)에 가까운 경우
//            diffuseColorToUse = glm::vec3(0.5f, 0.5f, 0.5f); // 중간 회색으로 강제 변경
//        }
//
//        glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(diffuseColorToUse));
//    }
//
//    if (!textures.empty()) {
//        glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(meshTexture.specularColor));
//        glUniform1i(glGetUniformLocation(shaderID, "shininess"), meshTexture.shininess);
//    }
//
//    //glm::vec3 exampleSpecular = glm::vec3(0.35f, 0.35f, 0.35f); // Ks 
//    //int exampleShininess = 32;                                   // Ns 
//    //glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(exampleSpecular));
//    //glUniform1i(glGetUniformLocation(shaderID, "shininess"), exampleShininess);
//
//    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
//}
//void StaticModel::SaveToBinary(const std::string& fileName) {
//    std::ofstream out(fileName, std::ios::out | std::ios::binary);
//    if (!out.is_open()) return;
//
//    // 1. 메쉬 개수
//    int meshCount = meshes.size();
//    out.write((char*)&meshCount, sizeof(int));
//
//    // 2. 메쉬 데이터
//    for (const auto& mesh : meshes) {
//        // 정점
//        int vSize = mesh.vertices.size();
//        out.write((char*)&vSize, sizeof(int));
//        out.write((char*)mesh.vertices.data(), vSize * sizeof(StaticVertex));
//
//        // 인덱스
//        int iSize = mesh.indices.size();
//        out.write((char*)&iSize, sizeof(int));
//        out.write((char*)mesh.indices.data(), iSize * sizeof(unsigned int));
//
//        // 텍스처 & 재질 정보
//        int texSize = mesh.textures.size();
//        out.write((char*)&texSize, sizeof(int));
//
//        for (const auto& tex : mesh.textures) {
//            // A. 문자열 정보 (Type, Path)
//            std::string type = tex.type;
//            int typeLen = type.length();
//            out.write((char*)&typeLen, sizeof(int));
//            out.write(type.c_str(), typeLen);
//
//            std::string path = tex.path;
//            int pathLen = path.length();
//            out.write((char*)&pathLen, sizeof(int));
//            out.write(path.c_str(), pathLen);
//
//            // ★ [추가] B. 재질 색상 정보 (Diffuse, Specular, Shininess)
//            // OBJ는 텍스처 없이 색상만 있는 경우가 많으므로 필수 저장
//            out.write((char*)&tex.diffuseColor, sizeof(glm::vec3));
//            out.write((char*)&tex.specularColor, sizeof(glm::vec3));
//            out.write((char*)&tex.shininess, sizeof(int));
//        }
//    }
//
//    out.close();
//    std::cout << "바이너리 저장 완료 (Static): " << fileName << std::endl;
//}
//bool StaticModel::LoadFromBinary(const std::string& fileName) {
//    std::ifstream in(fileName, std::ios::in | std::ios::binary);
//    if (!in.is_open()) return false;
//
//    int meshCount = 0;
//    in.read((char*)&meshCount, sizeof(int));
//
//    meshes.clear();
//    meshes.reserve(meshCount);
//
//    for (int i = 0; i < meshCount; i++) {
//        std::vector<StaticVertex> _vertices;
//        std::vector<unsigned int> _indices;
//        std::vector<Texture> _textures;
//
//        // 정점
//        int vSize = 0;
//        in.read((char*)&vSize, sizeof(int));
//        _vertices.resize(vSize);
//        in.read((char*)_vertices.data(), vSize * sizeof(StaticVertex));
//
//        // 인덱스
//        int iSize = 0;
//        in.read((char*)&iSize, sizeof(int));
//        _indices.resize(iSize);
//        in.read((char*)_indices.data(), iSize * sizeof(unsigned int));
//
//        // 텍스처 & 재질
//        int texSize = 0;
//        in.read((char*)&texSize, sizeof(int));
//
//        for (int k = 0; k < texSize; k++) {
//            Texture tex;
//
//            // A. 문자열 읽기
//            int typeLen = 0;
//            in.read((char*)&typeLen, sizeof(int));
//            std::string type(typeLen, '\0');
//            in.read(&type[0], typeLen);
//            tex.type = type;
//
//            int pathLen = 0;
//            in.read((char*)&pathLen, sizeof(int));
//            std::string path(pathLen, '\0');
//            in.read(&path[0], pathLen);
//            tex.path = path;
//
//            // ★ [추가] B. 재질 색상 읽기
//            in.read((char*)&tex.diffuseColor, sizeof(glm::vec3));
//            in.read((char*)&tex.specularColor, sizeof(glm::vec3));
//            in.read((char*)&tex.shininess, sizeof(int));
//
//            // C. 텍스처 파일 로딩 (경로가 있을 때만)
//            if (!tex.path.empty()) {
//                std::string fullPath = this->directory + '/' + tex.path;
//                tex.id = loadTextureFromFile(tex.path.c_str(), this->directory); // 인자 1개 버전 사용 시 수정 필요
//                // 만약 loadTextureFromFile 인자가 2개라면: loadTextureFromFile(tex.path.c_str(), this->directory);
//            }
//            else {
//                tex.id = 0; // 텍스처 없음
//            }
//
//            _textures.push_back(tex);
//        }
//
//        StaticMesh mesh;
//        mesh.vertices = _vertices;
//        mesh.indices = _indices;
//        mesh.textures = _textures;
//        mesh.setupMesh(); // ★ 필수: VAO/VBO 생성
//
//        meshes.push_back(mesh);
//    }
//
//    in.close();
//    std::cout << "바이너리 로드 성공 (Static): " << fileName << std::endl;
//    return true;
//}
//// ===================================================================
//// StaticModel
//// ===================================================================
//StaticModel::StaticModel(const std::string& objPath) {
//    // 1. 디렉토리 설정 (매우 중요)
//    directory = objPath.substr(0, objPath.find_last_of('/'));
//    if (directory == objPath) directory = "."; // 슬래시가 없으면 현재 경로
//
//    // 2. 바이너리 파일 확인 및 로딩 시도
//    std::string binPath = objPath.substr(0, objPath.find_last_of('.')) + ".bin";
//
//    if (LoadFromBinary(binPath)) {
//        return; // 성공 시 Assimp 스킵
//    }
//
//    // =========================================================
//    // 3. 실패 시 Assimp로 로딩 (느림)
//    // =========================================================
//    Assimp::Importer importer;
//    // Tangent가 필요 없다면 CalcTangentSpace 옵션 빼도 됨 (속도 향상)
//    const aiScene* scene = importer.ReadFile(objPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
//
//    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
//        return;
//    }
//
//    processNode(scene->mRootNode, scene);
//
//    // 4. 로딩 완료 후 저장 (다음 실행을 위해)
//    SaveToBinary(binPath);
//}
//
//void StaticModel::Draw(GLuint shaderID) {
//    for (auto& mesh : meshes) mesh.Draw(shaderID);
//}
//
//void StaticModel::processNode(aiNode* node, const aiScene* scene)
//{
//    // 현재 노드가 가진 모든 메시를 처리합니다.
//    for (unsigned int i = 0; i < node->mNumMeshes; i++)
//    {
//        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//        meshes.push_back(processMesh(mesh, scene));
//    }
//
//    // 자식 노드를 재귀적으로 순회하며 처리합니다.
//    for (unsigned int i = 0; i < node->mNumChildren; i++)
//    {
//        processNode(node->mChildren[i], scene);
//    }
//}
//
//StaticMesh StaticModel::processMesh(aiMesh* mesh, const aiScene* scene)
//{
//    std::vector<StaticVertex> vertices;
//    std::vector<unsigned int> indices;
//    std::vector<Texture> textures; // 텍스처 로딩 구현 필요
//
//    // 1. 정점 데이터 추출
//    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
//    {
//        StaticVertex vertex;
//        // 위치 (Position)
//        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
//        // 법선 (Normal)
//        if (mesh->mNormals)
//            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
//        // 텍스처 좌표 (TexCoords)
//        if (mesh->mTextureCoords[0])
//            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
//        else
//            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//
//        vertices.push_back(vertex);
//    }
//
//    // 2. 인덱스 데이터 (Faces) 추출
//    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
//    {
//        aiFace face = mesh->mFaces[i];
//        for (unsigned int j = 0; j < face.mNumIndices; j++)
//            indices.push_back(face.mIndices[j]);
//    }
//
//    // 3. 재질/텍스처 처리 (구현)
//    if (mesh->mMaterialIndex >= 0)
//    {
//        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//
//        Texture matInfo;
//
//        // 👇 Diffuse Color (Kd) 추출 코드 추가/복구!
//        aiColor4D color_d(1.f, 1.f, 1.f, 1.f); // 기본값 설정 (흰색)
//        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color_d)) {
//            matInfo.diffuseColor = glm::vec3(color_d.r, color_d.g, color_d.b);
//        }
//
//        // 3-1. Specular Color (Ks) 추출 및 저장
//        aiColor4D color_s(0.f, 0.f, 0.f, 1.f);
//        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color_s)) {
//            matInfo.specularColor = glm::vec3(color_s.r, color_s.g, color_s.b);
//        }
//        float shininess_val = 1.0f;
//        if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess_val)) {
//            matInfo.shininess = (int)shininess_val;
//        }
//        // 👇 Ks 값 강제 조정 (하이라이트 강도 줄이기)
//        if (glm::length(matInfo.specularColor) > 1.0f) { // Ks가 너무 강하면 (예: 1.0, 1.0, 1.0)
//            matInfo.specularColor *= 0.05f; // Specular 강도를 35%로 조정 (이전 사용자 설정 기반)
//        }
//
//        // 3-2. 텍스처 파일 경로 추출 및 로딩
//        aiString str;
//        // AI_MATKEY_TEXTURE_DIFFUSE: MTL 파일의 map_Kd (확산 텍스처) 경로를 찾습니다.
//        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &str))
//        {
//            // 경로가 있다면 텍스처 로딩 시도
//            matInfo.path = str.C_Str();
//            // 텍스처 로드 함수 호출 (현재 파일 경로를 기준으로 로드)
//            // OBJ 파일이 있는 "현재 디렉토리"를 가정하고 경로 전달
//            matInfo.id = loadTextureFromFile(matInfo.path.c_str(), this->directory);
//            matInfo.type = "texture_diffuse";
//        }
//        else {
//            // 텍스처 경로가 없는 경우, ID를 0으로 설정하여 텍스처를 사용하지 않도록 표시
//            matInfo.id = 0;
//            matInfo.type = "color_diffuse";
//        }
//
//        // 재질 정보를 메시의 첫 번째 텍스처 항목에 저장
//        textures.push_back(matInfo);
//    }
//
//    // 메시 객체 생성 및 설정
//    StaticMesh staticMesh;
//    staticMesh.vertices = vertices;
//    staticMesh.indices = indices;
//    staticMesh.textures = textures;
//    staticMesh.setupMesh(); // VAO/VBO/EBO 설정 호출
//
//    return staticMesh;
//}
//
//void StaticModel::set_obb(int idx) {
//    road_local_obb.center = glm::vec3(0.0f, 0.0f, 0.0f);;
//    road_local_obb.half_length = glm::vec3(ROAD_SIZE / 2, 0.1f, ROAD_SIZE / 2);
//    road_local_obb.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
//    road_local_obb.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
//    road_local_obb.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
//
//    if (idx == 0) { // 동
//        maze_obb_setup(true, true, true, true, false, false, true, true, true);
//    }
//    else if (idx == 1) { // 서
//    }
//    else if (idx == 2) { // 남
//    }
//    else if (idx == 3) { // 북
//    }
//    else if (idx == 4) { // ㅡ
//    }
//    else if (idx == 5) { // ㅣ
//    }
//    else if (idx == 6) { // ┌
//    }
//    else if (idx == 7) { // ┐
//    }
//    else if (idx == 8) { // └
//    }
//    else if (idx == 9) { // ┘
//    }
//    else if (idx == 10) { // ㅏ
//    }
//    else if (idx == 11) { // ㅓ
//    }
//    else if (idx == 12) { // ㅜ
//    }
//    else if (idx == 13) { // ㅗ
//    }
//    else if (idx == 14) { // +
//    }
//    else if (idx == 15) { // x
//    }
//}
//
//void StaticModel::maze_obb_setup(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h, bool i) {
//    bool corners[9] = { a, b, c, d, e, f, g, h, i };
//    int corner_idx = 0;
//
//    for (int i = -1; i < 2; i++) {
//        for (int j = -1; j < 2; j++) {
//            if (corners[corner_idx]) {
//                OBB obstacle;
//                float obb_half_size = ROAD_SIZE / 6;
//                obstacle.center = glm::vec3((j * obb_half_size) * 2, ROAD_SIZE / 2, (i * obb_half_size) * 2);
//                obstacle.half_length = glm::vec3(obb_half_size, ROAD_SIZE / 2, obb_half_size);
//                obstacle.u[0] = glm::vec3(1.0f, 0.0f, 0.0f);
//                obstacle.u[1] = glm::vec3(0.0f, 1.0f, 0.0f);
//                obstacle.u[2] = glm::vec3(0.0f, 0.0f, 1.0f);
//                obstacle_local_obb.push_back(obstacle);
//            }
//            corner_idx++;
//        }
//    }
//}

//-=====================================================================================================================================
//-=====================================================================================================================================
//-=====================================================================================================================================
//-=====================================================================================================================================
//-=====================================================================================================================================
//#define STB_IMAGE_IMPLEMENTATION
//#include "game_mode.h"
//
//// 셰이더 파일 경로 상수
//const char* STATIC_VERT = "static_vertex.glsl";
//const char* ANIMATED_VERT = "animated_vertex.glsl";
//const char* FRAGMENT_LIGHT = "fragment.glsl";
//
//using namespace std;
//
//
//
//// 생성자: 변수 초기값 설정
//game_mode::game_mode() {
//
//
//    lightPos = glm::vec3(0.0f, 2000.0f, 0.0f);
//    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
//    materialSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
//    ambientStrength = 0.1f;
//    shininess = 32;
//
//}
//
//game_mode::~game_mode() {
//    // Finish()가 호출되지 않고 파괴될 경우를 대비
//    // (일반적으로는 Finish에서 정리함)
//}
//
//// 1. 초기화 (기존 init 함수 내용)
//void game_mode::Init() {
//
//
//    // GLEW 초기화는 보통 Framework나 Main에서 한 번 하지만, 
//    // 여기서 셰이더 컴파일을 수행합니다.
//
//    cout << "[GameMode] Initializing..." << endl;
//
//    // 셰이더 로드
//    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
//    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);
//
//    // 모델 로드
//    loadModels();
//
//    // 카메라 위치
//    /*camPos = glm::vec3(start_x_pos, 5.0f, start_z_pos - 5.0f);
//    camTarget = glm::vec3(start_x_pos, 0.0f, start_z_pos+5.0f);*/
//    glm::vec3 targetPos = silverWolf.pos;
//
//
//    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
//    gameCamera.Init(targetPos);
//
//
//    glEnable(GL_DEPTH_TEST);
//}
//
//float deltatime;
//// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
//void game_mode::Update(float deltaTime) {
//    silverWolf.Update(deltaTime, gameCamera.camera_x_angle);
//    gameCamera.Update(deltaTime, silverWolf.pos);
//    silverwolf_maze_collision();
//    //카메라 고정
//    if (camera_fixed == false) glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
//
//    //과녁
//    target.update();
//
//    deltatime = deltaTime;
//}
//
//void game_mode::silverwolf_maze_collision() {
//
//    for (auto& block : maze.mazeBlocks) {
//        block.is_colliding = false;
//        if (check_collision(silverWolf.silverwolf_world_obb, block.road_world_obb)) {
//            block.is_colliding = true;
//        }
//    }
//
//    // 청크 단위 충돌
//    bool stop = false;
//    for (int i = 1; i < maze_y - 1; i++) {
//        for (int j = 1; j < maze_x - 1; j++) {
//            if (maze.mazeBlocks[(i * maze_x) + j].is_colliding == true) {
//                update_chunk(i, j, 2);
//                stop = true;
//                break;
//            }
//        }
//        if (stop) break;
//    }
//
//    // 청크 기준으로 장애물 충돌 검사
//    for (auto& block : maze.mazeBlocks) {
//        if (block.is_colliding == true) {
//            for (int j = 0; j < block.obstacle_world_obb.size(); j++) {
//                if (check_collision(silverWolf.silverwolf_world_obb, block.obstacle_world_obb[j])) {
//                    silverWolf.pos = silverWolf.old_pos;
//                    silverWolf.update_world_obb();
//                }
//            }
//        }
//    }
//}
//
//void game_mode::update_chunk(int y, int x, int size) {
//    int min_y = std::max(0, y - size);
//    int max_y = std::min(maze_y, y + size);
//    int min_x = std::max(0, x - size);
//    int max_x = std::min(maze_x, x + size);
//
//    for (int i = min_y; i <= max_y; i++) {
//        for (int j = min_x; j <= max_x; j++) {
//            maze.mazeBlocks[(i * maze_x) + j].is_colliding = true;
//        }
//    }
//
//    for (auto& t : target.targetBlocks) {
//        t.is_in_chunk = false;
//        if (t.modelMatrix[3].x > (min_x * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && t.modelMatrix[3].x < ((max_x + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
//            t.modelMatrix[3].y >(min_y * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && t.modelMatrix[3].y < ((max_y + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
//            t.is_in_chunk = true;
//        }
//    }
//}
//
//// 3. 그리기 (기존 drawScene 함수 내용)
//void game_mode::Draw() {
//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    // 뷰, 프로젝션 행렬 계산
//    glm::mat4 view = glm::lookAt(gameCamera.camPos, gameCamera.camTarget, gameCamera.camUp);
//    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);
//
//    // static 모델 그리기
//    glUseProgram(shaderProgramStatic);
//    setCommonUniforms(shaderProgramStatic, view, proj);
//
//    for (auto& block : maze.mazeBlocks) {   //미로
//        if (block.is_colliding) {
//            glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(block.modelMatrix));
//            if (block.modelPtr)
//            {
//                // 메시별 재질/색상 정보 설정 및 드로우
//                for (auto& mesh : block.modelPtr->meshes)
//                {
//                    // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
//                    mesh.Draw(shaderProgramStatic);
//                }
//            }
//        }
//    }
//
//    for (auto& t : target.targetBlocks) {   //과녁
//        if (t.is_break || !t.is_in_chunk) continue;
//        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(t.modelMatrix));
//        if (t.modelPtr)
//        {
//            // 메시별 재질/색상 정보 설정 및 드로우
//            for (auto& mesh : t.modelPtr->meshes)
//            {
//                // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
//                mesh.Draw(shaderProgramStatic);
//            }
//        }
//    }
//
//    // 디버그용 OBB 그리기
//    glLineWidth(3.0f);        // 선 굵기 설정
//
//    for (auto& block : maze.mazeBlocks) {   // 미로 obb
//        if (block.is_colliding) {
//            if (block.modelPtr) {
//                drawDebugOBB(shaderProgramStatic, block.road_world_obb, view, proj, glm::vec3(0.0f, 1.0f, 0.0f)); // 초록색
//
//                for (int i = 0; i < block.obstacle_world_obb.size(); i++) {
//                    drawDebugOBB(shaderProgramStatic, block.obstacle_world_obb[i], view, proj, glm::vec3(1.0f, 1.0f, 0.0f)); // 노란색
//                }
//            }
//        }
//    }
//
//    for (auto& t : target.targetBlocks) {   // 과녁 obb
//        if (t.is_break || !t.is_in_chunk) continue;
//        drawDebugOBB(shaderProgramStatic, t.target_obb, view, proj, glm::vec3(0.0f, 0.0f, 1.0f)); // 파란색
//    }
//
//
//    // --- 2. 애니메이션 캐릭터 ---
//    glUseProgram(shaderProgramAnimated);
//    setCommonUniforms(shaderProgramAnimated, view, proj);
//
//    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
//    silverWolf.Draw(shaderProgramAnimated, deltatime);
//    if (!silverWolf.init_success) silverWolf.init_success = true;
//    drawDebugOBB(shaderProgramStatic, silverWolf.silverwolf_world_obb, view, proj, glm::vec3(1.0f, 0.0f, 0.0f)); // 빨간색
//}
//
//// 4. 정리 (종료 시 메모리 해제)
//void game_mode::Finish() {
//    // 로드된 도로 모델들 삭제
//    for (auto p : roads) {
//        delete p;
//    }
//    roads.clear();
//
//    // 늑대 모델 삭제 (silver_wolf 클래스 내부 구조에 따라 다를 수 있음)
//    // NewModel* 포인터들을 가지고 있다면 여기서 delete 해주는 것이 좋음
//    for (int i = 0; i < 7; ++i) {
//        if (silverWolf.silverWolfModel[i])
//            delete silverWolf.silverWolfModel[i];
//    }
//
//    // 셰이더 프로그램 삭제
//    glDeleteProgram(shaderProgramStatic);
//    glDeleteProgram(shaderProgramAnimated);
//}
//
//void game_mode::Reshape(int w, int h) {
//    WINDOW_WIDTH = w;
//    WINDOW_HEIGHT = h;
//    glViewport(0, 0, w, h);
//}
//
//void game_mode::OnPause() {
//    // 옵션 창 등을 열었을 때 멈춰야 할 로직이 있다면 여기에 작성
//}
//
//void game_mode::OnResume() {
//    // 옵션 창 닫고 돌아왔을 때 복구할 로직
//    glEnable(GL_DEPTH_TEST); // 혹시 다른 씬에서 껐을까봐 다시 켬
//}
//
//// ==========================================================
//// [Helper Functions] 기존 main.cpp의 함수들을 멤버 함수로 이식
//// ==========================================================
//
//void game_mode::loadModels() {
//    // 미로
//    roads.push_back(new StaticModel("road/road0.obj"));   // 0동
//    roads.push_back(new StaticModel("road/road1.obj"));   // 1서 
//    roads.push_back(new StaticModel("road/road2.obj"));   // 2남 
//    roads.push_back(new StaticModel("road/road3.obj"));   // 3북
//    roads.push_back(new StaticModel("road/road4.obj"));   // 4ㅡ
//    roads.push_back(new StaticModel("road/road5.obj"));   // 5ㅣ
//    roads.push_back(new StaticModel("road/road6.obj"));   // 6┌
//    roads.push_back(new StaticModel("road/road7.obj"));   // 7┐
//    roads.push_back(new StaticModel("road/road8.obj"));   // 8└
//    roads.push_back(new StaticModel("road/road9.obj"));   // 9┘
//    roads.push_back(new StaticModel("road/road10.obj"));  // 10ㅏ
//    roads.push_back(new StaticModel("road/road11.obj"));  // 11ㅓ
//    roads.push_back(new StaticModel("road/road12.obj"));  // 12ㅜ
//    roads.push_back(new StaticModel("road/road13.obj"));  // 13ㅗ
//    roads.push_back(new StaticModel("road/road14.obj"));  // 14+
//    roads.push_back(new StaticModel("road/road15.obj"));  // 15x
//    for (int i = 0; i < roads.size(); i++) {
//        roads[i]->set_maze_obb(i);
//    }
//    maze.setMaze();
//    maze.initmaze(&roads);
//
//    //과녁
//    target_model = new StaticModel("target/baudrive.obj");
//    target_model->set_target_obb();
//    target.init(target_model, target_count);
//    set_taret_in_maze();    //미로에 과녁 배치
//
//    //은랑
//    silverWolf.Init();
//
//    silverWolf.silverWolfModel[0] = new NewModel("silver_wolf/Idle.fbx");
//    silverWolf.silverWolfModel[0]->state = "idle";
//    silverWolf.silverWolfModel[1] = new NewModel("silver_wolf/Walk.fbx");
//    silverWolf.silverWolfModel[1]->state = "walk";
//    silverWolf.silverWolfModel[2] = new NewModel("silver_wolf/Running.fbx");
//    silverWolf.silverWolfModel[2]->state = "run";
//    silverWolf.silverWolfModel[3] = new NewModel("silver_wolf/Throw.fbx");
//    silverWolf.silverWolfModel[3]->state = "throw";
//    silverWolf.silverWolfModel[4] = new NewModel("silver_wolf/Jump Over.fbx");
//    silverWolf.silverWolfModel[4]->state = "roll";
//    silverWolf.silverWolfModel[5] = new NewModel("silver_wolf/Jump.fbx");
//    silverWolf.silverWolfModel[5]->state = "jump";
//    silverWolf.silverWolfModel[6] = new NewModel("silver_wolf/Running Jump.fbx");
//    silverWolf.silverWolfModel[6]->state = "jump_run";
//    silverWolf.silverWolfModel[7] = new NewModel("silver_wolf/Run To Stop.fbx");
//    silverWolf.silverWolfModel[7]->state = "stop_run";
//    silverWolf.silverWolfModel[8] = new NewModel("silver_wolf/Backflip.fbx");
//    silverWolf.silverWolfModel[8]->state = "jump_idle";
//
//
//}
//
//void game_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
//    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
//    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(gameCamera.camPos));
//
//    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
//    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
//    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);
//
//    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
//    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
//}
//
//char* game_mode::filetobuf(const char* file) {
//    ifstream f(file, ios::binary);
//    if (!f.is_open()) {
//        cerr << "ERROR: Cannot open shader file: " << file << endl;
//        return nullptr;
//    }
//    f.seekg(0, ios::end);
//    int len = (int)f.tellg();
//    char* buf = new char[len + 1];
//    f.seekg(0, ios::beg);
//    f.read(buf, len);
//    buf[len] = '\0';
//    f.close();
//    return buf;
//}
//
//void game_mode::loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
//    GLuint vertShader, fragShader;
//    GLint success;
//    GLchar infoLog[512];
//
//    // Vertex Shader
//    char* vertCode = filetobuf(vertPath);
//    if (!vertCode) return;
//    vertShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertShader, 1, &vertCode, NULL);
//    glCompileShader(vertShader);
//    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
//        cerr << "ERROR::VERTEX::COMPILATION_FAILED: " << vertPath << "\n" << infoLog << endl;
//    }
//    delete[] vertCode;
//
//    // Fragment Shader
//    char* fragCode = filetobuf(fragPath);
//    if (!fragCode) { glDeleteShader(vertShader); return; }
//    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragShader, 1, &fragCode, NULL);
//    glCompileShader(fragShader);
//    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
//        cerr << "ERROR::FRAGMENT::COMPILATION_FAILED: " << fragPath << "\n" << infoLog << endl;
//    }
//    delete[] fragCode;
//
//    // Program Link
//    shaderID = glCreateProgram();
//    glAttachShader(shaderID, vertShader);
//    glAttachShader(shaderID, fragShader);
//    glLinkProgram(shaderID);
//    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
//    if (!success) {
//        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
//        cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
//    }
//
//    glDeleteShader(vertShader);
//    glDeleteShader(fragShader);
//}
//
//void game_mode::Keyboard(unsigned char key, int x, int y) {
//
//    silverWolf.Keyboard(key, x, y);
//
//    switch (key)
//    {
//    case'g':
//        camera_fixed = true;
//        glutSetCursor(GLUT_CURSOR_INHERIT);
//        break;
//    }
//
//}
//void game_mode::Keyupboard(unsigned char key, int x, int y) {
//
//    silverWolf.Keyupboard(key, x, y);
//
//    switch (key)
//    {
//    case 27:
//        exit(0);
//        break;
//    case'g':
//        camera_fixed = false;
//        //glutSetCursor(GLUT_CURSOR_NONE);
//        break;
//    }
//}
//
//void game_mode::SpecialKeyboard(int key, int x, int y) {
//    silverWolf.SpecialKeyboard(key, x, y);
//
//
//}
//
//void game_mode::SpecialUpKeyboard(int key, int x, int y) {
//    silverWolf.SpecialUpKeyboard(key, x, y);
//}
//
//void game_mode::Mouse(int button, int state, int x, int y) {
//    silverWolf.Mouse(button, state, x, y);
//}
//
//void game_mode::PassiveMotion(int x, int y) {
//    if (silverWolf.init_success)
//        gameCamera.PassiveMotion(x, y);
//}
//
//void game_mode::set_taret_in_maze() {  // 과녁에서 미로 객체를 가져올 수 없어서 여기서 배치해줌
//    uniform_int_distribution<int> rd_x(1, maze_x - 2);
//    uniform_int_distribution<int> rd_y(1, maze_y - 2);
//
//    for (int i = 0; i < target.targetBlocks.size();i++) {
//
//        while (1) {
//            int rand_x = rd_x(mt), rand_y = rd_y(mt);
//            bool rd_flag = false;
//            if (maze.maze[rand_y][rand_x].path_wall == WALL || maze.maze[rand_y][rand_x].type == 15 || (rand_y == 1 && rand_x == 1)) continue;
//            for (int j = 0; j < i;j++) {
//                if (target.targetBlocks[j].modelMatrix == maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix) {
//                    rd_flag = true;
//                    break;
//                }
//            }
//            if (rd_flag) continue;
//
//            target.targetBlocks[i].modelMatrix = maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix;
//            target.targetBlocks[i].reset = glm::vec3(target.targetBlocks[i].modelMatrix[3][0], ROAD_SIZE / 4, target.targetBlocks[i].modelMatrix[3][2]); // 위치 저장
//            cout << rand_y << "," << rand_x << endl;
//            break;
//        }
//    }
//
//}

