#include "clear_mode.h"
#include "game_mode.h"
#include "option_mode.h"
#include "game_framwork.h"
#include "account.h"
#include "title_mode.h"

#define STB_IMAGE_IMPLEMENTATION

// 생성자: 변수 초기값 설정
clear_mode::clear_mode() {
    lightPos = glm::vec3(0.0f, 2000.0f, 0.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    materialSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    ambientStrength = 0.3f;
    shininess = 32;
    bgmChannel->stop();
    bgmChannel = soundManager.Play("qte", glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);

    scene_progress = 0;
    event_progress = 0;
}

clear_mode::~clear_mode() {
    // Finish()가 호출되지 않고 파괴될 경우를 대비
    // (일반적으로는 Finish에서 정리함)
}

void clear_mode::Finish() {
    if (trainer) {
        delete trainer;
        trainer = nullptr;
    }

    if (skybox) {
        delete skybox;
        skybox = nullptr;
    }

    if (black_bar) {
        delete black_bar;
        black_bar = nullptr;
    }

    if (black_background) {
        delete black_background;
        black_background = nullptr;
    }

    if (qte_f) {
        delete qte_f;
        qte_f = nullptr;
    }

    if (qte_tip) {
        delete qte_tip;
        qte_tip = nullptr;
    }

    for (auto img : scene_1) {
        delete img;
    }
    scene_1.clear();

    for (auto img : scene_2) {
        delete img;
    }
    scene_2.clear();

    // 셰이더 프로그램 삭제
    glDeleteProgram(shaderProgramStatic);
    glDeleteProgram(shaderProgramAnimated);
    glDeleteProgram(shaderProgramSkybox);
    glDeleteProgram(shaderProgramImage);
    glDeleteProgram(shaderProgramText);
}

void clear_mode::loadModels() {
    for (int i = 0;i <= 6;i++) {
        MazeBlockInstance roadInstance;
        if (i == 6) roadInstance.modelPtr = roads.at(2);
        else roadInstance.modelPtr = roads.at(5);
        roadInstance.modelMatrix = glm::mat4(1.0f);
        roadInstance.reset = glm::vec3(0.0f, 0.0f, -(ROAD_SIZE * i) + (ROAD_SIZE * 2));
        mazeBlocks.push_back(roadInstance);
    }
    start_x_pos = 0.0f;
    start_z_pos = 0.0f;

    chest.modelPtr = chest_model;
    chest.reset = mazeBlocks[6].reset + glm::vec3(0.0f, 0.0f, -2.5f);
    chest.modelMatrix = glm::mat4(1.0f);
    chest.modelMatrix = glm::translate(chest.modelMatrix, chest.reset);
    chest.modelMatrix = glm::scale(chest.modelMatrix, glm::vec3(0.2f)); //크기 조절

    silverWolf.Init();
    silverWolf.Keyboard('s', 0.0f, 0.0f);
    silverWolf.SpecialKeyboard(GLUT_KEY_SHIFT_L, 0.0f, 0.0f);

    trainer = new Trainer(0.0f, 5.0f);
    //trainer->Init();
    trainer->trainerModel[0] = press_model[0];
    trainer->trainerModel[1] = press_model[1];
    trainer->trainerModel[2] = press_model[2];
    trainer->scale = glm::vec3(0.005f);
    trainer->aggravation = true;

    // 스카이박스
    skybox = new Skybox("skybox/sun.png", "skybox/moon.png");

    // 카메라 위치
    glm::vec3 targetPos = silverWolf.pos + glm::vec3(0.0f, 1.0f, 0.0f);
    gameCamera.Init(targetPos);
    gameCamera.camTarget = silverWolf.pos + glm::vec3(0.0f, 0.0f, -40.0f);

    glEnable(GL_DEPTH_TEST);
}
void clear_mode::loadImages() {
    stbi_set_flip_vertically_on_load(true);

    glm::vec2 size1 = glm::vec2((float)1200, (float)800);
    glm::vec2 pos1 = glm::vec2((float)1200 / 2.0f, (float)800 / 2.0f);

    black_background = new Image(LoadTexture("scene_image/black_background.png"), pos1, size1);\
    black_background->color.w = 0.0f;

    black_bar = new Image(LoadTexture("scene_image/cut_scene_black_bar.png"), pos1, size1);
    black_bar->color.w = 1.0f;

    qte_f = new Image(LoadTexture("scene_image/qte_f.png"), glm::vec2(940.0f, 360.0f), glm::vec2(150.0f, 150.0f));
    qte_f->color.w = 1.0f;

    qte_tip = new Image(LoadTexture("scene_image/qte_tip.png"), pos1, size1);
    qte_tip->color.w = 1.0f;

    scene_1.push_back(new Image(LoadTexture("scene_image/1-1.png"), pos1, size1));
    scene_1.push_back(new Image(LoadTexture("scene_image/1-2.png"), pos1, size1));
    scene_1.push_back(new Image(LoadTexture("scene_image/1-3.png"), pos1, size1));
    scene_1.push_back(new Image(LoadTexture("scene_image/1-4.png"), pos1, size1));
    scene_1.push_back(new Image(LoadTexture("scene_image/1-5.png"), pos1, size1));
    for (auto& m : scene_1)m->color.w = 1.0f;

    scene_2.push_back(new Image(LoadTexture("scene_image/2-1.png"), pos1, size1));
    scene_2.push_back(new Image(LoadTexture("scene_image/2-2.png"), pos1, size1));
    for (auto& m : scene_2)m->color.w = 1.0f;

    stbi_set_flip_vertically_on_load(false);
}
void clear_mode::reshape_ui(float w, float h) {
	black_background->size = glm::vec2(w, h);
	black_background->position = glm::vec2(w / 2.0f, h / 2.0f);

	black_bar->size = glm::vec2(w, h);
	black_bar->position = glm::vec2(w / 2.0f, h / 2.0f);

	qte_f->position = glm::vec2(940.0f + ui_dis.x, 360.0f + ui_dis.y);

	qte_tip->position = glm::vec2(w / 2.0f, h / 2.0f);

    for (auto& m : scene_1) {
        m->position = glm::vec2(w / 2.0f, m->size.y /2 + ui_dis.y/4);
	}

    for (auto& m : scene_2) {
		m->size = glm::vec2(w, h);
        m->position = glm::vec2(w / 2.0f, m->size.y / 2 + ui_dis.y / 4);
	}
}
void clear_mode::loadTexts() {
}

// 1. 초기화 (기존 init 함수 내용)
void clear_mode::Init() {
    cout << "[clearMode] Initializing..." << endl;

    stbi_set_flip_vertically_on_load(false);

    if (bgmChannel) {
        bgmChannel->stop();
    }
    if (playerChannel) {
        playerChannel->stop();
    }
    soundManager.Update();

    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);
    shaderProgramImage = LoadShader(IMAGE_VERT, IMAGE_FRAG);
    loadShader("vertex_sky.glsl", "fragment_sky.glsl", shaderProgramSkybox);
    shaderProgramText = LoadShader(TEXT_VERT, TEXT_FRAG);

    loadModels();
    loadImages();
    loadTexts();

	ui_dis = glm::vec2((winWidth - 1200) / 2, (winHeight - 800) / 2);
	reshape_ui((float)winWidth, (float)winHeight);
}

// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void clear_mode::Update(float deltaTime) {
    bool isPlaying = false;
    if (bgmChannel) {
        bgmChannel->isPlaying(&isPlaying);
    }

    if (!isPlaying) {
        bgmChannel = soundManager.Play("qte", glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
    }

    if (pause) return;

    if (scene_progress == 0) {
        objects_Update(deltaTime);
        trainer->pos = glm::vec3(0.0f, 0.0f, 5.0f);

        if (event_progress == 0) {
            static float event_timer = 0.0f;
            event_timer += deltaTime;
            if (event_timer >= 10.0f) {
                event_progress++;
                playerChannel = soundManager.Play("clear_1", silverWolf.pos, effect_volume);
            }
        }

        if (event_progress == 6) {

            static glm::vec3 start_pos = gameCamera.camTarget;
            static glm::vec3 middle_pos = glm::vec3(
                -ROAD_SIZE * 2,
                (gameCamera.camTarget.y + trainer->pos.y) / 2,
                (gameCamera.camTarget.z + trainer->pos.z) / 2);
            static glm::vec3 end_pos = trainer->pos;
            static glm::vec3 cam_start_pos = gameCamera.camPos;
            static glm::vec3 cam_end_pos = glm::vec3(0.0f, 1.0f, -1.0f);


            static bool flip = false;
            static float timer = 0.0f;
            timer += deltaTime;
            if (timer >= 5.0f) {
                timer = 0.0f;
                flip = true;
            }
            float t = timer;
            if (t >= 1.0f) t = 1.0f;
            float one_minus_t = 1.0f - t;
            if (!flip) {
                gameCamera.camTarget = pow(one_minus_t, 2) * start_pos + (2.0f * one_minus_t * t) * middle_pos + pow(t, 2) * end_pos;
                gameCamera.camPos = one_minus_t * cam_start_pos + t * cam_end_pos;
            }
            else {
                gameCamera.camTarget = pow(one_minus_t, 2) * end_pos + (2.0f * one_minus_t * t) * middle_pos + pow(t, 2) * start_pos;
                gameCamera.camPos = one_minus_t * cam_end_pos + t * cam_start_pos;
            }
            if (flip && timer >= 1.5f) {
                pause = true;
                black_background->color.w = 0.5f;
                scene_progress = 1;
                event_progress = 0;
                gameCamera.camPos = glm::vec3(0.0f, 0.47f, 0.025f);
                gameCamera.camTarget = silverWolf.pos + glm::vec3(0.0f, 0.0f, -40.0f);

                trainer->pos = glm::vec3(0.0f, 0.0f, -(ROAD_SIZE * 5 / 2));
            }
        }
        else {
            static float timer = 0.0f;
            timer += deltaTime * 1.5f;
            if (timer >= 0.5f) timer = 0.0f;
            float t = -1.6f * pow(timer - 0.25f, 2) + 0.1f;
            gameCamera.camPos = glm::vec3(0.0f, 0.47f + t, 0.025f);
            gameCamera.camTarget = silverWolf.pos + glm::vec3(0.0f, 0.0f, -40.0f);


        }
    }
    else if (scene_progress == 1) {
        silverWolf.Keyboard('s', 0.0f, 0.0f);
        silverWolf.SpecialKeyboard(GLUT_KEY_SHIFT_L, 0.0f, 0.0f);

        static float trainer_timer = 0.0f;

        if (qte_is_success == 0) {

            if (!qte_pause) {
                objects_Update(deltaTime);

                static float timer = 0.0f;
                timer += deltaTime * 1.5f;
                if (timer >= 0.5f) timer = 0.0f;
                float t = -1.6f * pow(timer - 0.25f, 2) + 0.1f;
                gameCamera.camPos = glm::vec3(0.0f, 0.47f + t, 0.025f);
                gameCamera.camTarget = silverWolf.pos + glm::vec3(0.0f, 0.0f, -40.0f);
            }


            if (!qte_pause) {
                trainer_timer += deltaTime;
                trainer->pos = glm::vec3(0.0f, 0.0f, -(ROAD_SIZE * 5 / 2) + ROAD_SIZE * trainer_timer);
            }

            if (!qte_pause && circle_radius != 0.0f && trainer_timer >= 2.4f) {
                qte_pause = true;
                circle_radius = 200.0f;
            }

            if (qte_pause) {
                playerChannel->stop();
                circle_radius -= 50.0f * deltaTime;
                if (circle_radius <= 0.0f) {
                    circle_radius = 0.0f;
                    qte_pause = false;
                    qte_is_success = -1;
                }
            }

        }
        else {
            gameCamera.camPos = glm::vec3(-2.0f, 0.8f, 0.0f);
            gameCamera.camTarget = glm::vec3(0.0f, 0.4f, 0.0f);

            objects_Update(deltaTime);


            trainer_timer += deltaTime;
            trainer->pos = glm::vec3(0.0f, 0.0f, -(ROAD_SIZE * 5 / 2) + ROAD_SIZE * trainer_timer);

            if (trainer_timer >= 7.0f) {
                trainer_timer = 0.0f;
                qte_is_success = 0;
                circle_radius = 200.0f;
                if (qte_success_count >= 3) {
                    pause = false;
                    qte_pause = false;
                    scene_progress = 2;
                }
            }
        }

    }
    else if (scene_progress == 2) {
        static bool arrive = false;
        //맵 이동
        static float move_road = 0.0f;
        move_road += deltaTime;
        if (move_road >= 4.0f) {
            move_road = 4.0f;
            arrive = true;
        }
        for (auto& block : mazeBlocks) {
            block.modelMatrix = glm::mat4(1.0f);
            block.modelMatrix = glm::translate(block.modelMatrix, glm::vec3(0.0f, 0.0f, ROAD_SIZE * move_road));
            block.modelMatrix = glm::translate(block.modelMatrix, block.reset);
        }
        chest.modelMatrix = glm::mat4(1.0f);
        chest.modelMatrix = glm::translate(chest.modelMatrix, glm::vec3(0.0f, 0.0f, ROAD_SIZE * move_road));
        chest.modelMatrix = glm::translate(chest.modelMatrix, chest.reset);
        chest.modelMatrix = glm::scale(chest.modelMatrix, glm::vec3(0.2f));

        // 은랑 업데이트
        silverWolf.Update(deltaTime, gameCamera.camera_x_angle, gameCamera.camera_y_angle, gameCamera.right_mouth, false);
        silverWolf.pos = glm::vec3(0.0f, 0.0f, 0.0f);

        trainer->Update(deltaTime, gameCamera.right_mouth, silverWolf.pos);
        trainer->pos = glm::vec3(0.0f, 0.0f, 5.0f);

        static float timer = 0.0f;
        timer += deltaTime * 1.5f;
        if (timer >= 0.5f) timer = 0.0f;
        float t = -1.6f * pow(timer - 0.25f, 2) + 0.1f;
        gameCamera.camPos = glm::vec3(0.0f, 0.47f + t, 0.025f);
        gameCamera.camTarget = silverWolf.pos + glm::vec3(0.0f, 0.0f, -40.0f);

        if (arrive) {
            black_background->color.w += deltaTime * 0.5f;
            if (black_background->color.w >= 1.0f) {
                g_Framework->sceneManager->Change_Mode(new title_mode());
            }
        }

    }

}

void clear_mode::objects_Update(float deltaTime) {
    //맵 이동
    static float move_road = 0.0f;
    move_road += deltaTime;
    if (move_road >= 1.0f) move_road = 0.0f;
    for (auto& block : mazeBlocks) {
        block.modelMatrix = glm::mat4(1.0f);
        block.modelMatrix = glm::translate(block.modelMatrix, glm::vec3(0.0f, 0.0f, ROAD_SIZE * move_road));
        block.modelMatrix = glm::translate(block.modelMatrix, block.reset);
    }

    // 은랑 업데이트
    silverWolf.Update(deltaTime, gameCamera.camera_x_angle, gameCamera.camera_y_angle, gameCamera.right_mouth, false);
    silverWolf.pos = glm::vec3(0.0f, 0.0f, 0.0f);

    // 트레이너 업데이트
    trainer->Update(deltaTime, gameCamera.right_mouth, silverWolf.pos);
}

// 3. 그리기 (기존 drawScene 함수 내용)
void clear_mode::Draw() {
    Fog_Update();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 뷰, 프로젝션 행렬 계산
    glm::mat4 view = glm::lookAt(gameCamera.camPos, gameCamera.camTarget, gameCamera.camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // static 모델 그리기
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    //for (auto& block : mazeBlocks) {   //미로
    //    glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(block.modelMatrix));
    //    if (block.modelPtr)
    //    {
    //        for (auto& mesh : block.modelPtr->meshes)
    //        {
    //            mesh.Draw(shaderProgramStatic);
    //        }
    //    }
    //}

    for (int i = 0; i < mazeBlocks.size();i++) {
        if (scene_progress != 2 && i == 6) continue;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(mazeBlocks[i].modelMatrix));
        if (mazeBlocks[i].modelPtr)
        {
            for (auto& mesh : mazeBlocks[i].modelPtr->meshes)
            {
                mesh.Draw(shaderProgramStatic);
            }
        }
    }

    if (scene_progress == 2 && chest.modelPtr) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(chest.modelMatrix));
        for (auto& mesh : chest.modelPtr->meshes) {
            mesh.Draw(shaderProgramStatic);
        }
    }

    // --- 2. 애니메이션 캐릭터 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
    silverWolf.Draw(shaderProgramAnimated, 0.0f, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));
    if (!silverWolf.init_success) silverWolf.init_success = true;
    if (pause || qte_pause) {
        if (silverWolf.thisChannel) {
            bool isPlaying = false;
            silverWolf.thisChannel->isPlaying(&isPlaying);
            if (isPlaying) {
                silverWolf.thisChannel->stop();
            }
        }
    }
    trainer->Draw(shaderProgramAnimated, 0.0f, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));

    // 스카이박스 그리기
    glUseProgram(shaderProgramSkybox);
    //setCommonUniforms(shaderProgramSkybox, view, proj);
    skybox->Draw(view, proj, shaderProgramSkybox);

    // --- 3. UI 그리기 ---
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 uiProj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);

    if (scene_progress == 0) {
        black_bar->Draw(shaderProgramImage, uiProj);
        if (event_progress > 0 && event_progress < 6) {
            scene_1[event_progress - 1]->Draw(shaderProgramImage, uiProj);
        }
    }
    else if (scene_progress == 1) {
        black_bar->Draw(shaderProgramImage, uiProj);

        if (pause) {
            black_background->Draw(shaderProgramImage, uiProj);
            qte_tip->Draw(shaderProgramImage, uiProj);
        }

        if (qte_pause) {
            qte_f->Draw(shaderProgramImage, uiProj);
            drawCircle(940.0f + ui_dis.x, 360.0f + ui_dis.y, circle_radius, glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
        }

        if (qte_is_success == 1) {
            scene_2[0]->Draw(shaderProgramImage, uiProj);
        }
        else if (qte_is_success == -1) {
            scene_2[1]->Draw(shaderProgramImage, uiProj);
        }
    }
    else if (scene_progress == 2) {
        black_bar->Draw(shaderProgramImage, uiProj);
        black_background->Draw(shaderProgramImage, uiProj);
    }


    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void clear_mode::Fog_Update() {
    // 뷰, 프로젝션 행렬 계산
    glm::mat4 view = glm::lookAt(gameCamera.camPos, gameCamera.camTarget, gameCamera.camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // static 모델 그리기
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);
    GLint fogColorLocStatic = glGetUniformLocation(shaderProgramStatic, "u_FogColor");
    GLint fogStartLocStatic = glGetUniformLocation(shaderProgramStatic, "u_FogStart");
    GLint fogEndLocStatic = glGetUniformLocation(shaderProgramStatic, "u_FogEnd");
    glUniform3f(fogColorLocStatic, 0.5f, 0.5f, 0.5f);
    glUniform1f(fogStartLocStatic, 20.0f);
    glUniform1f(fogEndLocStatic, 25.0f);

    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);
    GLint fogColorLocAnimated = glGetUniformLocation(shaderProgramAnimated, "u_FogColor");
    GLint fogStartLocAnimated = glGetUniformLocation(shaderProgramAnimated, "u_FogStart");
    GLint fogEndLocAnimated = glGetUniformLocation(shaderProgramAnimated, "u_FogEnd");
    glUniform3f(fogColorLocAnimated, 0.5f, 0.5f, 0.5f);
    glUniform1f(fogStartLocAnimated, 20.0f);
    glUniform1f(fogEndLocAnimated, 25.0f);
}

void clear_mode::Keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
    case 13:
        if (scene_progress == 0) {
            if (event_progress > 0 && event_progress < 6) {
                event_progress++;
                pause = false;

                if (event_progress == 2) playerChannel = soundManager.Play("clear_2", silverWolf.pos, effect_volume);
                else if (event_progress == 4) playerChannel = soundManager.Play("clear_3", silverWolf.pos, effect_volume);
                else if (event_progress == 5) playerChannel = soundManager.Play("clear_4", silverWolf.pos, effect_volume);
            }
        }
        else if (scene_progress == 1) {
            pause = false;
            black_background->color.w = 0.0f;
        }
        break;
    case 'f':
    case 'F':
        if (scene_progress == 1 && !pause) {
            if (circle_radius != 0.0f) {
                if (qte_pause && circle_radius > 65.0f && circle_radius < 85.0f) {
                    if (playerChannel) {
                        bool isPlaying = false;
                        playerChannel->isPlaying(&isPlaying);
                        if (isPlaying) {
                            playerChannel->stop();
                        }
                    }

                    qte_is_success = 1;
                    qte_success_count++;
                    qte_pause = false;
                    circle_radius = 0.0f;
                    silverWolf.Keyboard('f', 0.0f, 0.0f);
                    playerChannel = soundManager.Play("quest", silverWolf.pos, effect_volume);
                }
                else {
                    qte_is_success = -1;
                    qte_pause = false;
                    circle_radius = 0.0f;
                }
            }
        }
        break;
    }
}

void clear_mode::Keyupboard(unsigned char key, int x, int y) {
}

void clear_mode::SpecialKeyboard(int key, int x, int y) {
}

void clear_mode::SpecialUpKeyboard(int key, int x, int y) {
}

void clear_mode::Mouse(int button, int state, int x, int y) {
}

void clear_mode::PassiveMotion(int x, int y) {
}

void  clear_mode::Motion(int x, int y) {
}

void clear_mode::OnPause() {
    // 옵션 창 등을 열었을 때 멈춰야 할 로직이 있다면 여기에 작성
}

void clear_mode::OnResume() {
    // 옵션 창 닫고 돌아왔을 때 복구할 로직

    glEnable(GL_DEPTH_TEST); // 혹시 다른 씬에서 껐을까봐 다시 켬
}

void clear_mode::Reshape(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    ui_dis = glm::vec2((w - 1200) / 2, (h - 800) / 2);
    reshape_ui((float)w, (float)h);
    glViewport(0, 0, w, h);
}

void clear_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(gameCamera.camPos));

    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);

    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
}

char* clear_mode::filetobuf(const char* file) {
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

void clear_mode::loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
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

void clear_mode::drawCircle(float x, float y, float r, glm::vec4 color) {
    const int segments = 360;
    std::vector<float> vertices;

    for (int i = 0; i <= segments; i++) {
        float angle = glm::radians(static_cast<float>(i));
        vertices.push_back(x + r * cos(angle));
        vertices.push_back(y + r * sin(angle));
    }

    glUseProgram(shaderProgramImage);

    // ✅ 더미 흰색 텍스처 바인딩 (셰이더가 텍스처를 샘플링할 때 흰색 반환)
    GLuint whiteTexture;
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    unsigned char white[4] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // model 행렬
    GLint modelLoc = glGetUniformLocation(shaderProgramImage, "model");
    if (modelLoc != -1) {
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    }

    // projection 행렬
    glm::mat4 uiProj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramImage, "projection"), 1, GL_FALSE, glm::value_ptr(uiProj));

    // ✅ spriteColor 설정
    glUniform4fv(glGetUniformLocation(shaderProgramImage, "spriteColor"), 1, glm::value_ptr(color));

    GLuint tempVAO, tempVBO;
    glGenVertexArrays(1, &tempVAO);
    glGenBuffers(1, &tempVBO);

    glBindVertexArray(tempVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // ✅ 선 두께
    glLineWidth(5.0f);

    // ✅ 테두리만 그리기
    glDrawArrays(GL_LINE_LOOP, 0, segments + 1);

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &tempVBO);
    glDeleteVertexArrays(1, &tempVAO);

    // ✅ 텍스처 정리
    glDeleteTextures(1, &whiteTexture);
}