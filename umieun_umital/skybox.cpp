#include "skybox.h"

Skybox::Skybox(const std::string& cubemapPath1, const std::string& cubemapPath2)
{
	InitRenderData();
	textureID1 = LoadCubemapFromSingleImage(cubemapPath1);
    //textureID2 = LoadCubemapFromSingleImage(cubemapPath2);
}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteTextures(1, &textureID1);
    //glDeleteTextures(1, &textureID2);
}

void Skybox::InitRenderData()
{
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

GLuint Skybox::LoadCubemapFromSingleImage(const std::string& path)
{


    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Failed to load skybox image: " << path << std::endl;
        return textureID;
    }
    /*if (width % 4 != 0 || height % 3 != 0) {
        std::cout << "Warning: skybox image size is not multiple of 4x3 layout. width=" << width << " height=" << height << std::endl;
    }*/

    const int faceW = width / 4;
    const int faceH = height / 3;
    const int faceSize = std::min(faceW, faceH);

    //std::cout << "Skybox total size: " << width << "x" << height << " -> face: " << faceW << "x" << faceH << " channels=" << nrChannels << std::endl;

    GLenum externalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    GLenum internalFormat = (nrChannels == 4) ? GL_RGBA8 : GL_RGB8;

    // 전개도에서 각 face의 (col,row) 오프셋 (너 기존과 동일한 순서)
    // offsets: {col, row}
    struct Off { int col; int row; };
    Off offsets[6] = {
        {2,1}, // +X (right)
        {0,1}, // -X (left)
        {1,0}, // +Y (top)
        {1,2}, // -Y (bottom)
        {3,1}, // +Z (back)
        {1,1}  // -Z (front)
    };


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const int channels = nrChannels;
    std::vector<unsigned char> faceBuf(faceW * faceH * channels);

    for (int i = 0; i < 6; ++i) {
        int col = offsets[i].col;
        int row = offsets[i].row;

        for (int y = 0; y < faceH; ++y) {
            int srcRow = row * faceH + y;

            int srcStart = (srcRow * width + col * faceW) * channels;

            int destStart = (y * faceW) * channels;

            memcpy(&faceBuf[destStart], &data[srcStart], faceW * channels);
        }


        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat,
            faceW,
            faceH,
            0,
            externalFormat,
            GL_UNSIGNED_BYTE,
            faceBuf.data());

        
    }

    stbi_image_free(data);

   
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //std::cout << "Cubemap created (ID=" << textureID << ")" << std::endl;
    return textureID;
}

void Skybox::Draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderID)
{

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, &viewNoTrans[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    if(!change)
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID1);
	else
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID2);
    glUniform1i(glGetUniformLocation(shaderID, "skybox"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

}