#include "title_mode.h"
#include "game_mode.h"
#include "option_mode.h"
#include "game_framwork.h"
#include "account.h"

#define STB_IMAGE_IMPLEMENTATION

// 생성자: 변수 초기값 설정
title_mode::title_mode() {


    lightPos = glm::vec3(0.0f, 2000.0f, 0.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    materialSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    ambientStrength = 0.3f;
    shininess = 32;

    loading_start = false;
}

title_mode::~title_mode() {
    // Finish()가 호출되지 않고 파괴될 경우를 대비
    // (일반적으로는 Finish에서 정리함)
}

void title_mode::Finish() {
    // 로드된 도로 모델들 삭제
    for (auto p : roads) {
        delete p;
    }
    roads.clear();

    // 늑대 모델 삭제 (silver_wolf 클래스 내부 구조에 따라 다를 수 있음)
    // NewModel* 포인터들을 가지고 있다면 여기서 delete 해주는 것이 좋음
    for (int i = 0; i < 7; ++i) {
        if (silverWolf.silverWolfModel[i])
            delete silverWolf.silverWolfModel[i];
    }

    // 이미지 삭제
    if (white_background) {
        delete white_background;
    }
    if (title) {
        delete title;
    }
    if(loading_image) {
        delete loading_image;
	}

    // main 이미지들 삭제
    for (auto& m : main) {
        if (m) {
            delete m;
        }
    }
    main.clear();

    // set_maze 이미지들 삭제
    for (int i = 0; i < 4; ++i) {
        if (set_maze[i]) {
            delete set_maze[i];
        }
    }

    // 스카이박스 삭제
    if (skybox) {
        delete skybox;
    }

    // 셰이더 프로그램 삭제
    if (shaderProgramStatic) glDeleteProgram(shaderProgramStatic);
    if (shaderProgramAnimated) glDeleteProgram(shaderProgramAnimated);
    if (shaderProgramImage) glDeleteProgram(shaderProgramImage);
    if (shaderProgramSkybox) glDeleteProgram(shaderProgramSkybox);
    if (shaderProgramText) glDeleteProgram(shaderProgramText);
    stbi_set_flip_vertically_on_load(false);
}

void title_mode::loadModels() {
    // 미로
    roads.push_back(new StaticModel("road/road2.obj"));   // 2남 
    maze.title_maze(&roads);

    //은랑
    silverWolf.Init();

}
void title_mode::loadImages() {
    stbi_set_flip_vertically_on_load(true);

    glm::vec2 size1 = glm::vec2((float)winWidth, (float)winHeight);
    glm::vec2 pos1 = glm::vec2((float)winWidth / 2.0f, (float)winHeight / 2.0f);

    white_background = new Image(LoadTexture("title/white_background.png"), pos1, size1);
    white_background->color.w = 0.4f;

    title = new Image(LoadTexture("title/title.png"), pos1, size1);
    title->color.w = 1.0f;

	loading_image = new Image(LoadTexture("title/loading.png"), pos1, size1);
	loading_image->color.w = 1.0f;

    main.push_back(new Image(LoadTexture("title/main.png"), pos1, size1));
    main.push_back(new Image(LoadTexture("title/main_start.png"), pos1, size1));
    main.push_back(new Image(LoadTexture("title/main_option.png"), pos1, size1));
    main.push_back(new Image(LoadTexture("title/main_exit.png"), pos1, size1));
    for (auto& m : main)m->color.w = 1.0f;
    main_idx = 0;

    set_maze[0] = new Image(LoadTexture("title/set_maze.png"), pos1, size1);
    set_maze[1] = new Image(LoadTexture("title/set_maze_5x5.png"), pos1, size1);
    set_maze[2] = new Image(LoadTexture("title/set_maze_15x15.png"), pos1, size1);
    set_maze[3] = new Image(LoadTexture("title/set_maze_25x25.png"), pos1, size1);
   
    draw_set_maze = false;

    stbi_set_flip_vertically_on_load(false);
}
void title_mode::loadTexts() {
    glm::mat4 proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

    textUI.Init("font/neodgm.ttf", shaderProgramText, proj);
}

// 1. 초기화 (기존 init 함수 내용)
void title_mode::Init() {
    cout << "[TitleMode] Initializing..." << endl;

    // 셰이더 로드
    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);
    shaderProgramImage = LoadShader(IMAGE_VERT, IMAGE_FRAG);
    loadShader("vertex_sky.glsl", "fragment_sky.glsl", shaderProgramSkybox);
    shaderProgramText = LoadShader(TEXT_VERT, TEXT_FRAG);

    // 모델 로드
    loadModels();
    loadImages();
    loadTexts();

    // 스카이박스
    skybox = new Skybox("skybox/sun.png", "skybox/moon.png");

    // 배경음악 재생
    bgmChannel->stop();
    bgmChannel = soundManager.Play("title", glm::vec3(0.0f, 0.0f, 0.0f), bgm_volume);

    // 카메라 위치
    glm::vec3 targetPos = silverWolf.pos + glm::vec3(0.0f, 10.0f, -10.0f);


    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    gameCamera.Init(targetPos);
    gameCamera.camTarget = silverWolf.pos;
    camera_fixed = true;
    gameCamera.now_pos_idx = 0;
    gameCamera.moving = false;

    glEnable(GL_DEPTH_TEST);
}

float title_deltatime;
// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void title_mode::Update(float deltaTime) {
    soundManager.Update();
    silverWolf.Update(deltaTime, gameCamera.camera_x_angle, gameCamera.camera_y_angle, gameCamera.right_mouth);
    if (gameCamera.moving) gameCamera.title_update(deltaTime, silverWolf.pos, start_pos_idx, end_pos_idx);

    title_deltatime = deltaTime;
}

// 3. 그리기 (기존 drawScene 함수 내용)
void title_mode::Draw() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 뷰, 프로젝션 행렬 계산
    glm::mat4 view = glm::lookAt(gameCamera.camPos, gameCamera.camTarget, gameCamera.camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // static 모델 그리기
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    for (auto& block : maze.mazeBlocks) {   //미로
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(block.modelMatrix));
        if (block.modelPtr)
        {
            // 메시별 재질/색상 정보 설정 및 드로우
            for (auto& mesh : block.modelPtr->meshes)
            {
                // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
                mesh.Draw(shaderProgramStatic);
            }
        }
    }

    // --- 2. 애니메이션 캐릭터 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
    silverWolf.Draw(shaderProgramAnimated, title_deltatime, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));
    if (!silverWolf.init_success) silverWolf.init_success = true;

    // 스카이박스 그리기
    glUseProgram(shaderProgramSkybox);
    //setCommonUniforms(shaderProgramSkybox, view, proj);
    skybox->Draw(view, proj, shaderProgramSkybox);

    // --- 3. UI 그리기 ---
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 uiProj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
    if (!gameCamera.moving && gameCamera.now_pos_idx == 0) {
        white_background->Draw(shaderProgramImage, uiProj);
        title->Draw(shaderProgramImage, uiProj);
    }
    else if (!gameCamera.moving && gameCamera.now_pos_idx == 1) {
        main[main_idx]->Draw(shaderProgramImage, uiProj);
        if (draw_set_maze) {
            white_background->Draw(shaderProgramImage, uiProj);
            set_maze[set_maze_idx]->Draw(shaderProgramImage, uiProj);

            string score;
            score = account.best_score(0);
            textUI.Draw(score, 205.0f, 200.0f, 0.6f, glm::vec3(0.0f, 0.0f, 0.0f));
            score = account.best_score(1);
            textUI.Draw(score, 600.0f, 200.0f, 0.6f, glm::vec3(0.0f, 0.0f, 0.0f));
            score = account.best_score(2);
            textUI.Draw(score, 985.0f, 200.0f, 0.6f, glm::vec3(0.0f, 0.0f, 0.0f));

			if (loading_start) loading_image->Draw(shaderProgramImage, uiProj);
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void title_mode::Keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
	case 13:    // enter
        if (!gameCamera.moving && gameCamera.now_pos_idx == 0) {
            playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);
            start_pos_idx = 0;
            end_pos_idx = 1;
            gameCamera.moving = true;
        }
        break;
	case 27:    // esc
        if (!gameCamera.moving && gameCamera.now_pos_idx == 0) {
            exit(0);
        }
        else if (!gameCamera.moving && gameCamera.now_pos_idx == 1) {
            if (!draw_set_maze) {
                playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);
                start_pos_idx = 1;
                end_pos_idx = 0;
                gameCamera.moving = true;
            }
            else {
                playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);
                draw_set_maze = false;
            }
        }
        break;
    }
}

void title_mode::Keyupboard(unsigned char key, int x, int y) {
}

void title_mode::SpecialKeyboard(int key, int x, int y) {
}

void title_mode::SpecialUpKeyboard(int key, int x, int y) {
}

void title_mode::Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (gameCamera.now_pos_idx == 1) {
            if (draw_set_maze) {
                if (x >= 110 && x <= 360 && y >= 175 && y <= 625) {

                    maze_x = 5;
                    maze_y = 5;
                    loading_start = true;
                }
                else if (x >= 470 && x <= 720 && y >= 175 && y <= 625) {
                    maze_x = 15;
                    maze_y = 15;
                    loading_start = true;
                }
                else if (x >= 830 && x <= 1080 && y >= 175 && y <= 625) {
                    maze_x = 25;
                    maze_y = 25;
                    loading_start = true;
                }
            }
            else {
                if (x >= 600 && x <= 1200 && y >= 0 && y <= 800) {
                    uniform_int_distribution<int> rd_state(0, 1);
                    int click_state = rd_state(mt);

                    if (click_state == 0) silverWolf.Keyboard(' ', x, y);
                }
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (gameCamera.now_pos_idx == 1) {
            if (draw_set_maze) {
                if (x >= 110 && x <= 360 && y >= 175 && y <= 625) {
                    playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);



                    if (g_Framework && g_Framework->sceneManager) {
                        g_Framework->sceneManager->Change_Mode(new game_mode());
                    }
                }
                else if (x >= 470 && x <= 720 && y >= 175 && y <= 625) {
                    playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);



                    if (g_Framework && g_Framework->sceneManager) {
                        g_Framework->sceneManager->Change_Mode(new game_mode());
                    }
                }
                else if (x >= 830 && x <= 1080 && y >= 175 && y <= 625) {
                    playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);



                    if (g_Framework && g_Framework->sceneManager) {
                        g_Framework->sceneManager->Change_Mode(new game_mode());
                    }
                }
            }
            else {
                if (x >= 30 && x <= 450 && y >= 160 && y <= 240) { //start
                    playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);



                    draw_set_maze = true;
                    main_idx = 0;
                }
                else if (x >= 30 && x <= 450 && y >= 360 && y <= 440) { //option
                    playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);



                    g_Framework->sceneManager->Push_Mode(new option_mode());
                }
                else if (x >= 30 && x <= 450 && y >= 560 && y <= 640) {  //exit
                    exit(0);
                }
            }
        }
    }
}

void title_mode::PassiveMotion(int x, int y) {
    if (gameCamera.now_pos_idx == 1) {
        if (!draw_set_maze) {
            if (x >= 30 && x <= 450 && y >= 160 && y <= 240) { //start
                main_idx = 1;
            }
            else if (x >= 30 && x <= 450 && y >= 360 && y <= 440) { //option
                main_idx = 2;
            }
            else if (x >= 30 && x <= 450 && y >= 560 && y <= 640) {  //exit
                main_idx = 3;
            }
            else {
                main_idx = 0;
            }
        }
        else {
            if (x >= 110 && x <= 360 && y >= 175 && y <= 625) { 
                set_maze_idx = 1;
            }
            else if (x >= 470 && x <= 720 && y >= 175 && y <= 625) {
                set_maze_idx = 2;
            }
            else if (x >= 830 && x <= 1080 && y >= 175 && y <= 625) {
                set_maze_idx = 3;
            }
            else {
                set_maze_idx = 0;
            }
        }
    }
}

void  title_mode::Motion(int x, int y) {
}

void title_mode::OnPause() {
    // 옵션 창 등을 열었을 때 멈춰야 할 로직이 있다면 여기에 작성
}

void title_mode::OnResume() {
    // 옵션 창 닫고 돌아왔을 때 복구할 로직

    glEnable(GL_DEPTH_TEST); // 혹시 다른 씬에서 껐을까봐 다시 켬
}

void title_mode::Reshape(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    glViewport(0, 0, w, h);
}

void title_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(gameCamera.camPos));

    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);

    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
}

char* title_mode::filetobuf(const char* file) {
    ifstream f(file, ios::binary);
    if (!f.is_open()) {
        cerr << "ERROR: Cannot open shader file: " << file << endl;
        return nullptr;
    }
    f.seekg(0, ios::end);
    int len = (int)f.tellg();
    char* buf = new char[len + 1];
    f.seekg(0, ios::beg);
    f.read(buf, len);
    buf[len] = '\0';
    f.close();
    return buf;
}

void title_mode::loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
    GLuint vertShader, fragShader;
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    char* vertCode = filetobuf(vertPath);
    if (!vertCode) return;
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCode, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        cerr << "ERROR::VERTEX::COMPILATION_FAILED: " << vertPath << "\n" << infoLog << endl;
    }
    delete[] vertCode;

    // Fragment Shader
    char* fragCode = filetobuf(fragPath);
    if (!fragCode) { glDeleteShader(vertShader); return; }
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        cerr << "ERROR::FRAGMENT::COMPILATION_FAILED: " << fragPath << "\n" << infoLog << endl;
    }
    delete[] fragCode;

    // Program Link
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertShader);
    glAttachShader(shaderID, fragShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}