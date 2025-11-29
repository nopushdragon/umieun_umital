#pragma once
#include "headers.h" 

class Text
{
private:
    FT_Library ft_library; 
    FT_Face ft_face;       
public:
    struct Character {
        GLuint TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        GLuint Advance;
    };

    std::map<char, Character> Characters;
    GLuint VAO, VBO;
    GLuint shader;
    glm::mat4 projection;

public:
    Text();

    void Init(const char* fontPath, GLuint shaderProgram, glm::mat4 proj);
    void Draw(const std::string& text, float x, float y, float scale, glm::vec3 color);
private:
    void LoadGlyph(FT_ULong char_code);
};
GLuint LoadShader(const char* vertexPath, const char* fragmentPath);