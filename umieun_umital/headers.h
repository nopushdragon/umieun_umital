#pragma once
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <algorithm>
#include <cmath>
#include <Windows.h>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <random>
#include <sstream>
#include <array>
#include <map>
#include <string_view>
#include "fmod.hpp"
#include <thread>
#include <chrono>
#include <cctype>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp> // 쿼터니언 필수
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include <glm/ext.hpp>
#include <ft2build.h>
#include "stb_image.h"
#include FT_FREETYPE_H
using namespace std;
extern std::random_device rd;
extern std::mt19937 mt;

extern int winWidth;
extern int winHeight;

#define ROAD_SIZE 20.0f
extern int maze_y;
extern int maze_x;
extern float start_x_pos;
extern float start_z_pos;