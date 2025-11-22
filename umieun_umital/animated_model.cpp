#include "animated_model.h"

// -----------------------------------------------------------
// AssimpGLMHelpers 구현
// -----------------------------------------------------------
glm::mat4 AssimpGLMHelpers::ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

glm::vec3 AssimpGLMHelpers::GetGLMVec(const aiVector3D& vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat AssimpGLMHelpers::GetGLMQuat(const aiQuaternion& pOrientation) {
    return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

// -----------------------------------------------------------
// Mesh 구현
// -----------------------------------------------------------
Mesh::Mesh(vector<ModelVertex> vertices, vector<unsigned int> indices, vector<Texture_fbx> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

void Mesh::Draw(GLuint shaderID) {
    if (!textures.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
		glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), true);
        glBindTexture(GL_TEXTURE_2D, textures[0].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 1. Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)0);
    // 2. Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, Normal));
    // 3. TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, TexCoords));
    // 4. Bone IDs
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(ModelVertex), (void*)offsetof(ModelVertex, m_BoneIDs));
    // 5. Weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, m_Weights));

    glBindVertexArray(0);
}

// -----------------------------------------------------------
// NewModel 구현
// -----------------------------------------------------------
NewModel::NewModel(string const& path, bool gamma) : gammaCorrection(gamma) {
    // 초기화
    pos = glm::vec3(0.0f);
    scale = glm::vec3(0.05f);
    angle = 0.0f;
    m_BoneCounter = 0;

    loadModel(path);
}

void NewModel::Draw(GLuint shaderID, float currentTime) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, scale);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix3fv(glGetUniformLocation(shaderID, "uModelNormal"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(model)))));

    vector<glm::mat4> transforms;
    BoneTransform(currentTime, transforms);

    for (unsigned int i = 0; i < transforms.size(); ++i) {
        string name = "uFinalBoneMatrices[" + to_string(i) + "]";
        glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(transforms[i]));
    }

    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shaderID);
}

void NewModel::loadModel(string const& path) {
    m_Scene = m_Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights| aiProcess_FlipUVs);

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
        cout << "ERROR::ASSIMP:: " << m_Importer.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    if (directory == "") directory = path.substr(0, path.find_last_of('\\'));

    m_GlobalInverseTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(m_Scene->mRootNode->mTransformation);
    m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);

    RemoveRootMotion();
    processNode(m_Scene->mRootNode, m_Scene);
}

void NewModel::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh NewModel::processMesh(aiMesh* mesh, const aiScene* scene) {
    vector<ModelVertex> vertices;
    vector<unsigned int> indices;
    vector<Texture_fbx> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        ModelVertex vertex;
        SetVertexBoneDataToDefault(vertex);

        vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
        if (mesh->HasNormals())
            vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture_fbx> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    ExtractBoneWeightForVertices(vertices, mesh, scene);

	//cout << vertices.size() << " " << indices.size() << endl;

    return Mesh(vertices, indices, textures);
}

// 텍스처 로딩 함수 (수정된 버전: FBX 경로 사용)
vector<Texture_fbx> NewModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
    vector<Texture_fbx> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture_fbx texture;
            string filename = to_string(currentTextureNum) + ".png";
            string fullPath = directory + '/' + filename;
            texture.id = TextureFromFile(fullPath.c_str());

            if (texture.id == 0) texture.id = TextureFromFile(filename.c_str());

            if (texture.id != 0) {
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
                //cout << "[Success] Loaded: " << filename << endl;
            }
        }
    }
    if (currentTextureNum < 6) currentTextureNum++;
    return textures;
}


unsigned int NewModel::TextureFromFile(const char* path) {
    string filename = string(path);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = (nrComponents == 1) ? GL_RED : (nrComponents == 3) ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        return textureID;
    }
    else {
        stbi_image_free(data);
        return 0;
    }
}

// 뼈 헬퍼 함수들
void NewModel::SetVertexBoneDataToDefault(ModelVertex& vertex) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

void NewModel::SetVertexBoneData(ModelVertex& vertex, int boneID, float weight) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.m_BoneIDs[i] < 0) {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            break;
        }
    }
}

void NewModel::ExtractBoneWeightForVertices(vector<ModelVertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        int boneID = -1;
        string boneName = mesh->mBones[boneIndex]->mName.C_Str();

        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else {
            boneID = m_BoneInfoMap[boneName].id;
        }

        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

// 보간 함수들
glm::mat4 NewModel::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumPositionKeys == 1)
        return glm::translate(glm::mat4(1.0f), AssimpGLMHelpers::GetGLMVec(pNodeAnim->mPositionKeys[0].mValue));

    unsigned int index = 0;
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            index = i;
            break;
        }
    }
    unsigned int nextIndex = (index + 1);
    float deltaTime = (float)(pNodeAnim->mPositionKeys[nextIndex].mTime - pNodeAnim->mPositionKeys[index].mTime);
    float factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[index].mTime) / deltaTime;
    factor = glm::clamp(factor, 0.0f, 1.0f);
    const aiVector3D& start = pNodeAnim->mPositionKeys[index].mValue;
    const aiVector3D& end = pNodeAnim->mPositionKeys[nextIndex].mValue;
    aiVector3D delta = start + factor * (end - start);
    return glm::translate(glm::mat4(1.0f), AssimpGLMHelpers::GetGLMVec(delta));
}

glm::mat4 NewModel::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumRotationKeys == 1) {
        aiQuaternion r = pNodeAnim->mRotationKeys[0].mValue;
        return glm::toMat4(glm::quat(r.w, r.x, r.y, r.z));
    }
    unsigned int index = 0;
    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            index = i;
            break;
        }
    }
    unsigned int nextIndex = (index + 1);
    float deltaTime = (float)(pNodeAnim->mRotationKeys[nextIndex].mTime - pNodeAnim->mRotationKeys[index].mTime);
    float factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[index].mTime) / deltaTime;
    factor = glm::clamp(factor, 0.0f, 1.0f);
    const aiQuaternion& start = pNodeAnim->mRotationKeys[index].mValue;
    const aiQuaternion& end = pNodeAnim->mRotationKeys[nextIndex].mValue;
    aiQuaternion out;
    aiQuaternion::Interpolate(out, start, end, factor);
    out = out.Normalize();
    return glm::toMat4(glm::quat(out.w, out.x, out.y, out.z));
}

glm::mat4 NewModel::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim) {
    if (pNodeAnim->mNumScalingKeys == 1)
        return glm::scale(glm::mat4(1.0f), AssimpGLMHelpers::GetGLMVec(pNodeAnim->mScalingKeys[0].mValue));

    unsigned int index = 0;
    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            index = i;
            break;
        }
    }
    unsigned int nextIndex = (index + 1);
    float deltaTime = (float)(pNodeAnim->mScalingKeys[nextIndex].mTime - pNodeAnim->mScalingKeys[index].mTime);
    float factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[index].mTime) / deltaTime;
    factor = glm::clamp(factor, 0.0f, 1.0f);
    const aiVector3D& start = pNodeAnim->mScalingKeys[index].mValue;
    const aiVector3D& end = pNodeAnim->mScalingKeys[nextIndex].mValue;
    aiVector3D delta = start + factor * (end - start);
    return glm::scale(glm::mat4(1.0f), AssimpGLMHelpers::GetGLMVec(delta));
}

void NewModel::RemoveRootMotion() {
    if (!m_Scene || m_Scene->mNumAnimations == 0) return;
    aiAnimation* pAnimation = m_Scene->mAnimations[0];
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        string nodeName(pNodeAnim->mNodeName.data);
        string lowerName = nodeName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (i == 0 ||
            lowerName.find("hips") != string::npos ||
            lowerName.find("root") != string::npos ||
            lowerName.find("pelvis") != string::npos ||
            lowerName.find("armature") != string::npos ||
            lowerName.find("mixamo") != string::npos)
        {
            for (unsigned int k = 0; k < pNodeAnim->mNumPositionKeys; k++) {
                aiVector3D& pos = pNodeAnim->mPositionKeys[k].mValue;
                pos.x = 0.0f;
                pos.z = 0.0f;
            }
        }
    }
}

void NewModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& parentTransform) {
    string nodeName(pNode->mName.data);
    const aiAnimation* pAnimation = m_Scene->mAnimations[0];
    glm::mat4 nodeTransformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(pNode->mTransformation);
    const aiNodeAnim* pNodeAnim = nullptr;

    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        if (string(pAnimation->mChannels[i]->mNodeName.data) == nodeName) {
            pNodeAnim = pAnimation->mChannels[i];
            break;
        }
    }

    if (pNodeAnim) {
        glm::mat4 Scaling = CalcInterpolatedScaling(AnimationTime, pNodeAnim);
        glm::mat4 Rotation = CalcInterpolatedRotation(AnimationTime, pNodeAnim);
        glm::mat4 Translation = CalcInterpolatedPosition(AnimationTime, pNodeAnim);

        string lowerName = nodeName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });
        if (lowerName.find("hips") != string::npos ||
            lowerName.find("root") != string::npos ||
            lowerName.find("pelvis") != string::npos)
        {
            glm::vec3 pos = glm::vec3(Translation[3]);
            Translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, pos.y, 0.0f));
        }
        nodeTransformation = Translation * Rotation * Scaling;
    }

    glm::mat4 GlobalTransformation = parentTransform * nodeTransformation;

    if (m_BoneInfoMap.find(nodeName) != m_BoneInfoMap.end()) {
        int BoneIndex = m_BoneInfoMap[nodeName].id;
        m_BoneInfoMap[nodeName].finalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfoMap[nodeName].offset;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void NewModel::BoneTransform(float timeInSeconds, vector<glm::mat4>& Transforms) {
    glm::mat4 Identity = glm::mat4(1.0f);
    float TicksPerSecond = 25.0f;
    float Duration = 0.0f;

    if (m_Scene->mNumAnimations > 0) {
        TicksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f);
        Duration = (float)m_Scene->mAnimations[0]->mDuration;
    }
    else {
        return;
    }

    float TimeInTicks = timeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, Duration);

    ReadNodeHeirarchy(AnimationTime, m_Scene->mRootNode, Identity);

    Transforms.resize(100);
    for (int i = 0; i < 100; i++) Transforms[i] = glm::mat4(1.0f);

    for (auto const& [name, info] : m_BoneInfoMap) {
        if (info.id < 100) {
            Transforms[info.id] = info.finalTransformation;
        }
    }
}

NewModel::~NewModel() {
	// Assimp Importer will automatically clean up
}