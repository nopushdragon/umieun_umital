#pragma once
#include "headers.h"
class Skybox
{
public:
	Skybox(const std::string& cubemapPath1, const std::string& cubemapPath2);
	~Skybox();
    void Draw(const glm::mat4& view, const glm::mat4& projection, GLuint shaderID);

    bool change = false;
	float day_timer = 0.0f;
private:
    GLuint skyboxVAO, skyboxVBO;
    GLuint textureID1;
    GLuint textureID2;

    void InitRenderData();
    GLuint LoadCubemapFromSingleImage(const std::string& path);
};

