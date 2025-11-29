#include "Text.h"
#include "Image.h"
//텍스트 ㄱㄱ
Text textUI;

Image* uiImage = nullptr;
Image* uiImage2 = nullptr;
Image* uiImage3 = nullptr;
Image* uiImage4[8];


void Init_text()
{
	GLuint fontShader = LoadShader("vertex_text.glsl", "fragment_text.glsl");
	glm::mat4 proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

	textUI.Init("경기천년제목_Medium.ttf", fontShader, proj);


}


void Init_Image()
{
	stbi_set_flip_vertically_on_load(true);
	uiShaderProgram = LoadShader("vertex_image.glsl", "fragment_image.glsl");


	GLuint imageTextureID = LoadTexture("main.png");


	if (imageTextureID) {

		glm::vec2 size1 = glm::vec2((float)width, (float)height);
		glm::vec2 pos1 = glm::vec2((float)width / 2.0f, (float)height / 2.0f);
		uiImage = new Image(imageTextureID, pos1, size1);
		uiImage->color.w = 1.0f;

		GLuint imageTextureID2 = LoadTexture("f_press.png");
		glm::vec2 pos2 = glm::vec2(600.0f, 50.0f);
		glm::vec2 size2 = glm::vec2(825.0f / 2.0f, 216.0f / 2.0f);
		uiImage2 = new Image(imageTextureID2, pos2, size2);

		GLuint imageTextureID3 = LoadTexture("read.png");
		glm::vec2 size3 = glm::vec2(800.0f, (float)height);
		glm::vec2 pos3 = glm::vec2(600.0f, -500);
		uiImage3 = new Image(imageTextureID3, pos3, size3);
		uiImage3->color.w = 0.5f;

		for (int i = 0;i < 8;++i) {
			string path = "gif/frame_000" + to_string(i + 1) + ".png";
			GLuint imageTextureID4 = LoadTexture(path.c_str());
			glm::vec2 size4 = glm::vec2((float)width, (float)height);
			glm::vec2 pos4 = glm::vec2((float)width / 2.0f + 100.0f, (float)height / 2.0f);
			uiImage4[i] = new Image(imageTextureID4, pos4, size4);
		}

		//uiImage3
	}
}


드로우

glm::mat4 uiProj = glm::ortho(200.0f, (float)width, 0.0f, (float)height);
if (uiImage2) uiImage2->Draw(uiShaderProgram, uiProj);
if (uiImage3) uiImage3->Draw(uiShaderProgram, uiProj);
if (h) {
	static int t = 0;
	++t;
	if (t % frame_speed == 0) {
		frame_num = (frame_num + 1) % 8;
		t = 0;
	}
	if (uiImage4[frame_num]) uiImage4[frame_num]->Draw(uiShaderProgram, uiProj);
}

if (!game_start) {
	std::string prompt;
	if (is_input_width) {
		prompt = "WIDTH: ";
	}
	else {
		prompt = "HEIGHT: ";
	}
	prompt += input_buffer;
	textUI.Draw(prompt, width / 2.0f - 200.0f, 200, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}


GLuint LoadTexture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;

		if (nrComponents == 4) {
			internalFormat = GL_RGBA;
			dataFormat = GL_RGBA;
		}
		else if (nrComponents == 3) {
			internalFormat = GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 1) {
			internalFormat = GL_RED;
			dataFormat = GL_RED;
		}
		else {
			internalFormat = GL_RGB;
			dataFormat = GL_RGB;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (nrComponents == 1) {
			GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return 0;
	}

	return textureID;
}
