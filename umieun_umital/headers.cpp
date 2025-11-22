#include "headers.h"

//animated_model하고 static_model에서 쓰임
unsigned int loadTextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    // Assimp가 제공하는 텍스처 경로는 보통 상대 경로이므로, OBJ/MTL 파일이 있는 디렉토리와 결합해야 합니다.
    // 현재는 OBJ 파일이 'road0.obj'이므로, 텍스처 'color_128x128.jpg'가 같은 디렉토리에 있다고 가정하고 경로 결합은 생략합니다.
    // filename = directory + '/' + filename; // 만약 텍스처가 서브폴더에 있다면 주석 해제

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // stbi_load: 이미지 파일을 로드하는 핵심 함수
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        // glTexImage2D: 이미지 데이터를 OpenGL 텍스처로 전송
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 텍스처 래핑 및 필터링 설정 (일반적인 설정)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cerr << "ERROR::TEXTURE::Failed to load texture at path: " << filename << std::endl;
        textureID = 0; // 로드 실패 시 0 반환
    }

    // stbi_image_free: 이미지 데이터를 해제 (더 이상 필요 없음)
    stbi_image_free(data);
    return textureID;
}

// main에서 쓰임
char* filetobuf(const char* file) {
    ifstream f(file, ios::binary);
    if (!f.is_open()) {
        cerr << "ERROR: Cannot open shader file: " << file << endl;
        return nullptr;
    }
    f.seekg(0, ios::end);
    int len = f.tellg();
    char* buf = new char[len + 1];
    f.seekg(0, ios::beg);
    f.read(buf, len);
    buf[len] = '\0';
    f.close();
    return buf;
}
void loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
    GLuint vertShader, fragShader;
    GLint success;
    GLchar infoLog[512];

    // 1. 버텍스 셰이더 컴파일
    const char* vertCode = filetobuf(vertPath);
    if (!vertCode) return;
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCode, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << vertPath << "\n" << infoLog << endl;
    }
    delete[] vertCode;

    // 2. 프래그먼트 셰이더 컴파일
    const char* fragCode = filetobuf(fragPath);
    if (!fragCode) { glDeleteShader(vertShader); return; }
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << fragPath << "\n" << infoLog << endl;
    }
    delete[] fragCode;

    // 3. 셰이더 프로그램 링크
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertShader);
    glAttachShader(shaderID, fragShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    // 셰이더 객체는 프로그램에 링크된 후 삭제 가능
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}