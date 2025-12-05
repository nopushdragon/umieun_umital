#define STB_IMAGE_IMPLEMENTATION
#include "game_mode.h"
#include "account.h"
#include "game_framwork.h"
#include "title_mode.h"

// 셰이더 파일 경로 상수
const char* STATIC_VERT = "static_vertex.glsl";
const char* ANIMATED_VERT = "animated_vertex.glsl";
const char* FRAGMENT_LIGHT = "fragment.glsl";
const char* IMAGE_VERT = "vertex_image.glsl";
const char* IMAGE_FRAG = "fragment_image.glsl";
const char* TEXT_VERT = "vertex_text.glsl";
const char* TEXT_FRAG = "fragment_text.glsl";

using namespace std;
uniform_int_distribution<int> random_bgm(0, 4);
uniform_int_distribution<int> random_player(0, 2);
uniform_int_distribution<int> random_trainer(0, 2);


// 생성자: 변수 초기값 설정
game_mode::game_mode() {


    lightPos = glm::vec3(0.0f, 200.0f, 0.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    materialSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    ambientStrength = 0.3f;
    shininess = 32;
    bgmChannel->stop();
    bgmChannel = soundManager.Play("main" + to_string(random_bgm(mt)), glm::vec3(0.0f, 0.0f, 0.0f), bgm_volume);

    camera_fixed = true;
    game_start = true;
    fade = false;
}

game_mode::~game_mode() {
    // Finish()가 호출되지 않고 파괴될 경우를 대비
    // (일반적으로는 Finish에서 정리함)
}

// 1. 초기화 (기존 init 함수 내용)
void game_mode::Init() {
    cout << "[GameMode] Initializing..." << endl;

    // 셰이더 로드
    loadShader(STATIC_VERT, FRAGMENT_LIGHT, shaderProgramStatic);
    loadShader(ANIMATED_VERT, FRAGMENT_LIGHT, shaderProgramAnimated);
    loadShader("vertex_sky.glsl", "fragment_sky.glsl", shaderProgramSkybox);
    shaderProgramImage = LoadShader(IMAGE_VERT, IMAGE_FRAG);
    shaderProgramText = LoadShader(TEXT_VERT, TEXT_FRAG);

    // 모델 로드
    loadModels();

    // ui 및 텍스트 로드
    loadImages();
    loadTexts();

    // 스카이박스
    skybox = new Skybox("skybox/sun.png", "skybox/moon.png");

    // 카메라 위치
    glm::vec3 targetPos = silverWolf.pos;

    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    gameCamera.Init(targetPos);

    playerChannel = soundManager.Play("quest", silverWolf.pos, effect_volume);

    glEnable(GL_DEPTH_TEST);
}

float deltatime;
// 2. 업데이트 (기존 timer 함수 내용 중 로직 부분)
void game_mode::Update(float deltaTime) {
    if (ultimate) {
        ultimate_frame += 0.3f;
        //ultimate_frame += 0.2f;
        //cout<< ultimate_frame << endl;
        if (ultimate_frame >= 103.0f) {
            ultimate_frame = 0.0f;
            ultimate = false;
            for (int i = trainers.size() - 1;i >= 0;--i) {
                delete trainers[i];
                trainers.erase(trainers.begin() + i);
            }
        }
        return;
    }

    if (fade) {
        black_background->color.w += 1.0f * (deltaTime / 2.0f);
        if (black_background->color.w >= 1.0f) {
            g_Framework->sceneManager->Change_Mode(new title_mode());
        }
    }
    else {
        silverWolf.Update(deltaTime, gameCamera.camera_x_angle, gameCamera.camera_y_angle, gameCamera.right_mouth);
        gameCamera.Update(deltaTime, silverWolf.pos);
        //카메라 고정
        if (camera_fixed == false) glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

        //트레이너
        Update_Trainer_Spawn();

        silverwolf_chunk_collision();   //청크  
        trainer_chunk_collision();

        for (int i = 0;i < trainers.size();++i) {
            if (trainers[i]->is_in_chunk) trainers[i]->Update(deltaTime, gameCamera.right_mouth, silverWolf.pos);
        }

        //과녁
        target.update(deltaTime);

        for (auto& b : balls) {
            if (b.is_in_chunk) {
                b.Update(deltaTime);
            }
        }

        bool isPlaying = false;
        bgmChannel->isPlaying(&isPlaying);

        if (!isPlaying) {
            bgmChannel = soundManager.Play("main" + to_string(random_bgm(mt)), glm::vec3(0.0f, 0.0f, 0.0f), bgm_volume);
        }

        if (!game_start)record_time += deltaTime;

        glm::vec3 dir = gameCamera.camTarget - gameCamera.camPos;
        dir.y = 0.0f;
        glm::vec3 forward = glm::normalize(dir);
        soundManager.SetListenerAttributes(silverWolf.pos + glm::vec3(0.0f, 2.0f, 0.0f), forward, gameCamera.camUp, glm::vec3(0.0f, 0.0f, 0.0f));

    }
    soundManager.Update();
    deltatime = deltaTime;
}

void game_mode::silverwolf_chunk_collision() {

    // 먼저 모든 블록의 충돌 상태를 초기화
    for (auto& block : maze.mazeBlocks) {
        block.is_colliding = false;
    }

    // 플레이어와 충돌하는 블록 찾기
    int player_chunk_y = -1;
    int player_chunk_x = -1;

    for (int i = 0; i < maze_y; i++) {
        for (int j = 0; j < maze_x; j++) {
            if (check_collision(silverWolf.silverwolf_world_obb, maze.mazeBlocks[(i * maze_x) + j].road_world_obb)) {
                player_chunk_y = i;
                player_chunk_x = j;
                break;
            }
        }
        if (player_chunk_y != -1) break;
    }

    // 플레이어 주변 청크 업데이트
    if (player_chunk_y != -1 && player_chunk_x != -1) {
        update_chunk(player_chunk_y, player_chunk_x, 2);
    }

    // 청크 기준으로 장애물 충돌 검사
    if (!is_f1) {
        for (auto& block : maze.mazeBlocks) {
            if (block.is_colliding == true) {
                for (int j = 0; j < block.obstacle_world_obb.size(); j++) {
                    if (check_collision(silverWolf.silverwolf_world_obb, block.obstacle_world_obb[j])) {
                        silverWolf.pos = silverWolf.old_pos;
                        silverWolf.update_world_obb();
                    }
                }
            }
        }
    }

    show_e_key = false;
    for (auto& c : chest.chestBlocks) {
        if (!c.is_in_chunk) continue;
        c.is_nearby = false;
        if (check_collision(silverWolf.silverwolf_world_obb, c.chest_obb)) {
            silverWolf.pos = silverWolf.old_pos;
            silverWolf.update_world_obb();
        }
        if (check_collision(silverWolf.silverwolf_world_obb, c.chest_nearby_obb)) {
            c.is_nearby = true;
            show_e_key = true;
        }
    }

    for (auto& t : target.targetBlocks) {
        if (t.is_break || !t.is_in_chunk) continue;
        for (auto& b : silverWolf.ball) {
            if (!b.end_pos) {
                if (check_collision(b.ball_obb, t.target_obb)) {
                    t.is_break = true;
                    b.end_pos = true;
                    playerChannel = soundManager.Play("silverwolf" + to_string(random_player(mt)), glm::vec3(0.0f, 0.0f, 0.0f), effect_volume);
                    b.thisChannel = soundManager.Play("catch", b.current_pos, effect_volume);
                    kill_count++;
                }
            }
        }
    }

    for (auto& b : balls) {
        if (!b.is_in_chunk) continue;
        b.is_nearby = false;
        if (check_collision(silverWolf.silverwolf_world_obb, b.ball_obb)) {
            b.is_nearby = true;
            show_e_key = true;
        }
    }


}

void game_mode::trainer_chunk_collision() {
    for (int i = trainers.size() - 1; i >= 0; --i) {
        Trainer* trainer = trainers[i];
        if (trainer->die) {
            delete trainer;
            trainers.erase(trainers.begin() + i);
            continue;
        }

        if (trainer->is_in_chunk == false) continue;

        // 장애물 충돌 검사
        for (auto& block : maze.mazeBlocks) {
            for (int j = 0; j < block.obstacle_world_obb.size(); j++) {
                if (check_collision(trainer->trainer_world_obb, block.obstacle_world_obb[j])) {
                    trainer->pos = trainer->old_pos;
                    trainer->angle += 45.0f;
                    trainer->update_world_obb();
                }
            }
        }

        // obb2번을 플레이어와 충돌할때
        if (check_collision(silverWolf.silverwolf_world_obb, trainer->trainer_world_obb2) && !trainer->die_change) {
            trainer->aggravation = true;
            if (!trainer->voice) {
                trainer->voice = true;
                bool playing = false;
                trainer->thisChannel->isPlaying(&playing);
                if (!playing)
                    trainer->thisChannel = soundManager.Play(to_string(trainer->radom_index) + to_string(random_trainer(mt)), trainer->pos, effect_volume);

            }
        }
        else {
            trainer->aggravation = false;
            trainer->voice = false;


        }

        // obb1번을 플레이어와 충돌할때
        if (check_collision(silverWolf.silverwolf_world_obb, trainer->trainer_world_obb) && !trainer->die_change) {
            if (silverWolf.state != "roll") {
                if (is_f1) {
                    silverWolf.pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
                    silverWolf.old_pos = glm::vec3(start_x_pos, 0.0f, start_z_pos);
                    delete trainer;
                    trainers.erase(trainers.begin() + i);
                    playerChannel = soundManager.Play("game_over", silverWolf.pos, effect_volume);
                }
            }
            else {
                trainer->die_change = true;
                playerChannel = soundManager.Play("attack", silverWolf.pos, effect_volume);
            }
        }
    }
}

void game_mode::update_chunk(int y, int x, int size) {
    int min_y = std::max(0, y - size);
    int max_y = std::min(maze_y - 1, y + size);
    int min_x = std::max(0, x - size);
    int max_x = std::min(maze_x - 1, x + size);

    for (int i = min_y; i <= max_y; i++) {
        for (int j = min_x; j <= max_x; j++) {
            maze.mazeBlocks[(i * maze_x) + j].is_colliding = true;
            maze.mazeBlocks[(i * maze_x) + j].was_colliding = true;
        }
    }

    for (int i = 0; i < target.targetBlocks.size(); i++) {
        target.targetBlocks[i].is_in_chunk = false;
        for (int j = min_y; j <= max_y; j++) {
            for (int k = min_x; k <= max_x; k++) {
                if (target.targetBlocks[i].modelMatrix[3].x > (k * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && target.targetBlocks[i].modelMatrix[3].x < ((k + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
                    target.targetBlocks[i].modelMatrix[3].z >(j * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && target.targetBlocks[i].modelMatrix[3].z < ((j + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
                    target.targetBlocks[i].is_in_chunk = true;
                    break;
                }
            }
            if (target.targetBlocks[i].is_in_chunk) break;
        }
    }

    for (int i = 0; i < chest.chestBlocks.size(); i++) {
        chest.chestBlocks[i].is_in_chunk = false;
        for (int j = min_y; j <= max_y; j++) {
            for (int k = min_x; k <= max_x; k++) {
                if (chest.chestBlocks[i].modelMatrix[3].x > (k * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && chest.chestBlocks[i].modelMatrix[3].x < ((k + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
                    chest.chestBlocks[i].modelMatrix[3].z >(j * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && chest.chestBlocks[i].modelMatrix[3].z < ((j + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
                    chest.chestBlocks[i].is_in_chunk = true;
                    break;
                }
            }
            if (chest.chestBlocks[i].is_in_chunk) break;
        }
    }

    for (int i = 0; i < balls.size(); i++) {
        balls[i].is_in_chunk = false;
        for (int j = min_y; j <= max_y; j++) {
            for (int k = min_x; k <= max_x; k++) {
                if (balls[i].current_pos.x > (k * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && balls[i].current_pos.x < ((k + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
                    balls[i].current_pos.z >(j * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && balls[i].current_pos.z < ((j + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
                    balls[i].is_in_chunk = true;
                    break;
                }
            }
            if (balls[i].is_in_chunk) break;
        }
    }

    for (int i = 0; i < trainers.size(); i++) {
        trainers[i]->is_in_chunk = false;
        for (int j = min_y; j <= max_y; j++) {
            for (int k = min_x; k <= max_x; k++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, trainers[i]->pos);
                model = glm::scale(model, trainers[i]->scale);
                model = glm::rotate(model, glm::radians(trainers[i]->angle), glm::vec3(0.0f, 1.0f, 0.0f));
                if (model[3].x > (k * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) && model[3].x < ((k + 1) * ROAD_SIZE - (ROAD_SIZE * maze_x) / 2) &&
                    model[3].z >(j * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2) && model[3].z < ((j + 1) * ROAD_SIZE - (ROAD_SIZE * maze_y) / 2)) {
                    trainers[i]->is_in_chunk = true;
                    break;
                }
            }
            if (trainers[i]->is_in_chunk) break;
        }
    }
}

void game_mode::Update_Trainer_Spawn() {
    trainer_spawn_timer += deltatime;
    if (trainer_spawn_timer >= trainer_spawn_time) {
        trainer_spawn_timer = 0.0f;
        Trainer* new_trainer = new Trainer(0.0f, 0.0f);
        new_trainer->Init();
        set_trainer_in_maze(*new_trainer);
        trainers.push_back(new_trainer);
    }
}

// 3. 그리기 (기존 drawScene 함수 내용)
void game_mode::Draw() {

    lightPos = silverWolf.pos + glm::vec3(0.0f, 200.0f, 0.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 뷰, 프로젝션 행렬 계산
    glm::mat4 view = glm::lookAt(gameCamera.camPos, gameCamera.camTarget, gameCamera.camUp);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);

    // static 모델 그리기
    glUseProgram(shaderProgramStatic);
    setCommonUniforms(shaderProgramStatic, view, proj);

    for (auto& block : maze.mazeBlocks) {   //미로
        if (block.is_colliding) {
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
    }

    for (auto& t : target.targetBlocks) {   //과녁
        if (t.is_break || !t.is_in_chunk) continue;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(t.modelMatrix));
        if (t.modelPtr)
        {
            // 메시별 재질/색상 정보 설정 및 드로우
            for (auto& mesh : t.modelPtr->meshes)
            {
                // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
                mesh.Draw(shaderProgramStatic);
            }
        }
    }

    for (auto& c : chest.chestBlocks) {   //보물
        if (!c.is_in_chunk) continue;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(c.modelMatrix));
        if (c.modelPtr)
        {
            // 메시별 재질/색상 정보 설정 및 드로우
            for (auto& mesh : c.modelPtr->meshes)
            {
                // 메시 그리기. 이제 Draw 함수가 재질 유니폼을 설정합니다.
                mesh.Draw(shaderProgramStatic);
            }
        }
    }

    for (auto& b : balls) {   // 몬스터볼 
        if (!b.is_in_chunk) continue;
        b.Draw(shaderProgramStatic);
    }


    silverWolf.Ball_Draw(shaderProgramStatic, deltatime, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));

    // --- 2. 애니메이션 캐릭터 ---
    glUseProgram(shaderProgramAnimated);
    setCommonUniforms(shaderProgramAnimated, view, proj);

    // 시간값 전달 (glutGet을 그대로 사용하거나, 누적 시간을 사용할 수 있음)
    silverWolf.Draw(shaderProgramAnimated, deltatime, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));
    if (!silverWolf.init_success) silverWolf.init_success = true;

    for (int i = 0;i < trainers.size();++i) {
        if (!trainers[i]->is_in_chunk) continue;
        trainers[i]->Draw(shaderProgramAnimated, deltatime, view, proj, glm::vec3(1.0f, 0.0f, 1.0f));
    }

    // --- 3. 스카이박스 ---
    glUseProgram(shaderProgramSkybox);
    setCommonUniforms(shaderProgramSkybox, view, proj);
    skybox->Draw(view, proj, shaderProgramSkybox);

    // --- 3. 오브젝트들 OBB ---
    if (collision_on) {
        glLineWidth(3.0f);        // 선 굵기 설정
        for (auto& block : maze.mazeBlocks) {   // 미로 obb
            if (block.is_colliding) {
                if (block.modelPtr) {
                    drawDebugOBB(shaderProgramStatic, block.road_world_obb, view, proj, glm::vec3(0.0f, 1.0f, 0.0f)); // 초록색

                    for (int i = 0; i < block.obstacle_world_obb.size(); i++) {
                        drawDebugOBB(shaderProgramStatic, block.obstacle_world_obb[i], view, proj, glm::vec3(1.0f, 1.0f, 0.0f)); // 노란색
                    }
                }
            }
        }

        for (auto& t : target.targetBlocks) {   // 과녁 obb
            if (t.is_break || !t.is_in_chunk) continue;
            drawDebugOBB(shaderProgramStatic, t.target_obb, view, proj, glm::vec3(0.0f, 0.0f, 1.0f)); // 파란색
        }

        for (auto& c : chest.chestBlocks) {   // 보물 obb
            if (!c.is_in_chunk) continue;
            drawDebugOBB(shaderProgramStatic, c.chest_obb, view, proj, glm::vec3(1.0f, 0.5f, 0.0f)); // 주황색
            drawDebugOBB(shaderProgramStatic, c.chest_nearby_obb, view, proj, glm::vec3(1.0f, 0.5f, 0.0f)); // 주황색
        }

        for (auto& b : balls) {   // 몬스터볼 obb
            if (!b.is_in_chunk) continue;
            drawDebugOBB(shaderProgramStatic, b.ball_obb, view, proj, glm::vec3(1.0f, 0.5f, 0.0f)); // 주황색
        }

        // 은랑 obb
        silverWolf.DebugOBB_Ball(shaderProgramStatic, view, proj, glm::vec3(1.0f, 0.0f, 1.0f)); // 보라색
        silverWolf.DebugOBB(shaderProgramAnimated, view, proj, glm::vec3(1.0f, 0.0f, 0.0f)); // 빨간색


        //트레이너
        for (int i = 0;i < trainers.size();++i) {
            if (!trainers[i]->is_in_chunk) continue;
            trainers[i]->DebugOBB(shaderProgramAnimated, view, proj, glm::vec3(0.0f, 0.0f, 1.0f)); // 파란색
        }


    }

    //ui 그리기
    drawMiniMap(WINDOW_WIDTH, WINDOW_HEIGHT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 uiProj = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);

    if (game_start || fade) black_background->Draw(shaderProgramImage, uiProj);
    if (game_start) mission->Draw(shaderProgramImage, uiProj);
    if (!game_start && !fade) {
        game_ui->Draw(shaderProgramImage, uiProj);
        if (show_e_key) e_key_image->Draw(shaderProgramImage, uiProj);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << record_time;
        string now_time = "Time: " + ss.str();
        textUI.Draw(now_time, 0, (WINDOW_HEIGHT * 5 / 6) - 100.0f, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));

        string ball_count = to_string(silverWolf.ball_cnt);
        textUI.Draw(ball_count, 1175.0f, 25.0f, 0.35f, glm::vec3(0.9f, 0.9f, 0.9f));

        string kill_count_str = to_string(kill_count);
        string enemy_count_str = to_string(target_count);
        textUI.Draw(kill_count_str + " / " + enemy_count_str, 50, (WINDOW_HEIGHT * 5 / 6) - 200.0f, 0.4f, glm::vec3(0.9f, 0.9f, 0.9f));

        if (ultimate) {
            silverWolfult[static_cast<int>(ultimate_frame)]->Draw(shaderProgramImage, uiProj);
            return;
        }
    }


    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void game_mode::drawMiniMap(int w, int h)
{
    // 미니맵 뷰포트 설정 (우측 상단)
    glViewport(0, h * 5 / 6, w / 6, h / 6);

    // 미니맵 영역만 깊이 버퍼 클리어
    glScissor(0, h * 5 / 6, w / 6, h / 6);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // 셰이더 프로그램 사용
    glUseProgram(shaderProgramStatic);

    float map_width = ROAD_SIZE * maze_x;
    float map_height = ROAD_SIZE * maze_y;
    float maxrange = std::max(map_width, map_height) / 2.0f;

    // 미니맵용 뷰 행렬 (위에서 내려다보기)
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, maxrange, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // 미니맵용 직교 투영 행렬
    glm::mat4 proj = glm::ortho(-maxrange, maxrange, -maxrange, maxrange, 0.1f, maxrange * 4.0f);

    // 미니맵용 유니폼 설정
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "viewPos"), 1, glm::value_ptr(glm::vec3(0.0f, maxrange, 0.0f)));

    // 조명을 완전히 끄고 원래 색상만 표시
    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "lightPos"), 1, glm::value_ptr(glm::vec3(0.0f, maxrange * 3.0f, 0.0f)));
    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform1f(glGetUniformLocation(shaderProgramStatic, "ambientStrength"), 1.0f); // ambient를 최대로 (조명 효과 없이 원색 표시)
    glUniform1i(glGetUniformLocation(shaderProgramStatic, "shininess"), 0);
    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "materialSpecular"), 1, glm::value_ptr(glm::vec3(0.0f)));

    // 미로 렌더링
    for (int i = 0; i < maze_y; i++) {
        for (int j = 0; j < maze_x; j++) {
            int index = i * maze_x + j;

            if (maze.mazeBlocks[index].was_colliding) {
                // 방문한 블록
                if (maze.maze[i][j].path_wall == WALL) {
                    // 벽은 검은색 큐브로 표시
                    glm::vec3 blockColor = glm::vec3(0.2f, 0.2f, 0.2f);
                    glUniform1i(glGetUniformLocation(shaderProgramStatic, "bUseTexture"), 0);
                    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "materialColorDefault"), 1, glm::value_ptr(blockColor));
                    glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE,
                        glm::value_ptr(maze.mazeBlocks[index].modelMatrix));

                    drawMiniMapCube(shaderProgramStatic, maze.mazeBlocks[index].modelMatrix);
                }
                else {
                    // 길은 실제 모델을 그림 (텍스처 포함)
                    glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE,
                        glm::value_ptr(maze.mazeBlocks[index].modelMatrix));

                    if (maze.mazeBlocks[index].modelPtr) {
                        for (auto& mesh : maze.mazeBlocks[index].modelPtr->meshes) {
                            mesh.Draw(shaderProgramStatic);
                        }
                    }
                }
            }
            else {
                // 방문하지 않은 블록 - 어두운 큐브로 표시
                glm::vec3 blockColor = glm::vec3(0.05f, 0.05f, 0.05f);
                glUniform1i(glGetUniformLocation(shaderProgramStatic, "bUseTexture"), 0);
                glUniform3fv(glGetUniformLocation(shaderProgramStatic, "materialColorDefault"), 1, glm::value_ptr(blockColor));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE,
                    glm::value_ptr(maze.mazeBlocks[index].modelMatrix));

                drawMiniMapCube(shaderProgramStatic, maze.mazeBlocks[index].modelMatrix);
            }
        }
    }

    // 플레이어 위치 표시 (빨간색 막대)
    glm::vec3 playerColor = glm::vec3(1.0f, 0.0f, 0.0f);
    glUniform1i(glGetUniformLocation(shaderProgramStatic, "bUseTexture"), 0);
    glUniform3fv(glGetUniformLocation(shaderProgramStatic, "materialColorDefault"), 1, glm::value_ptr(playerColor));
    glm::mat4 playerModel = glm::mat4(1.0f);
    playerModel = glm::translate(playerModel, silverWolf.pos);
    if (maze_x == 5) playerModel = glm::scale(playerModel, glm::vec3(0.2f, 2.0f, 0.2f));
    else if (maze_x == 15) playerModel = glm::scale(playerModel, glm::vec3(0.6f, 2.0f, 0.6f));
    else if (maze_x == 25) playerModel = glm::scale(playerModel, glm::vec3(1.0f, 2.0f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramStatic, "uModel"), 1, GL_FALSE, glm::value_ptr(playerModel));
    drawMiniMapCube(shaderProgramStatic, playerModel);

    // 메인 뷰포트로 복원
    glViewport(0, 0, w, h);
}

void game_mode::drawMiniMapCube(GLuint shaderID, const glm::mat4& modelMatrix) {
    float halfSize = ROAD_SIZE / 2.0f;

    const float cubeVertices[] = {
        // 위치 (x,y,z), 법선 (nx,ny,nz)
        // 뒷면 (Z-) - 법선: (0,0,-1)
        -halfSize, -halfSize, -halfSize,  0.0f, 0.0f, -1.0f,
         halfSize, -halfSize, -halfSize,  0.0f, 0.0f, -1.0f,
         halfSize,  halfSize, -halfSize,  0.0f, 0.0f, -1.0f,
         halfSize,  halfSize, -halfSize,  0.0f, 0.0f, -1.0f,
        -halfSize,  halfSize, -halfSize,  0.0f, 0.0f, -1.0f,
        -halfSize, -halfSize, -halfSize,  0.0f, 0.0f, -1.0f,

        // 앞면 (Z+) - 법선: (0,0,1)
        -halfSize, -halfSize,  halfSize,  0.0f, 0.0f, 1.0f,
         halfSize, -halfSize,  halfSize,  0.0f, 0.0f, 1.0f,
         halfSize,  halfSize,  halfSize,  0.0f, 0.0f, 1.0f,
         halfSize,  halfSize,  halfSize,  0.0f, 0.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  0.0f, 0.0f, 1.0f,
        -halfSize, -halfSize,  halfSize,  0.0f, 0.0f, 1.0f,

        // 왼쪽 면 (X-) - 법선: (-1,0,0)
        -halfSize,  halfSize,  halfSize, -1.0f, 0.0f, 0.0f,
        -halfSize,  halfSize, -halfSize, -1.0f, 0.0f, 0.0f,
        -halfSize, -halfSize, -halfSize, -1.0f, 0.0f, 0.0f,
        -halfSize, -halfSize, -halfSize, -1.0f, 0.0f, 0.0f,
        -halfSize, -halfSize,  halfSize, -1.0f, 0.0f, 0.0f,
        -halfSize,  halfSize,  halfSize, -1.0f, 0.0f, 0.0f,

        // 오른쪽 면 (X+) - 법선: (1,0,0)
         halfSize,  halfSize,  halfSize,  1.0f, 0.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  1.0f, 0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  1.0f, 0.0f, 0.0f,
         halfSize, -halfSize, -halfSize,  1.0f, 0.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  1.0f, 0.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  1.0f, 0.0f, 0.0f,

         // 아래 면 (Y-) - 법선: (0,-1,0)
         -halfSize, -halfSize, -halfSize,  0.0f, -1.0f, 0.0f,
          halfSize, -halfSize, -halfSize,  0.0f, -1.0f, 0.0f,
          halfSize, -halfSize,  halfSize,  0.0f, -1.0f, 0.0f,
          halfSize, -halfSize,  halfSize,  0.0f, -1.0f, 0.0f,
         -halfSize, -halfSize,  halfSize,  0.0f, -1.0f, 0.0f,
         -halfSize, -halfSize, -halfSize,  0.0f, -1.0f, 0.0f,

         // 위 면 (Y+) - 법선: (0,1,0)
         -halfSize,  halfSize, -halfSize,  0.0f, 1.0f, 0.0f,
          halfSize,  halfSize, -halfSize,  0.0f, 1.0f, 0.0f,
          halfSize,  halfSize,  halfSize,  0.0f, 1.0f, 0.0f,
          halfSize,  halfSize,  halfSize,  0.0f, 1.0f, 0.0f,
         -halfSize,  halfSize,  halfSize,  0.0f, 1.0f, 0.0f,
         -halfSize,  halfSize, -halfSize,  0.0f, 1.0f, 0.0f
    };

    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDisableVertexAttribArray(2);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
}

void game_mode::Keyboard(unsigned char key, int x, int y) {

    if (ultimate)return;

    if (key == 13) { // Enter key
        if (game_start) {
            playerChannel = soundManager.Play("click", silverWolf.pos, effect_volume);

            game_start = false;
            camera_fixed = false;
        }
    }
    if (game_start || fade) return;

    silverWolf.Keyboard(key, x, y);

    switch (key)
    {
    case 9:
        collision_on = !collision_on;
        break;
    case'g':
    case'G':
        camera_fixed = true;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        break;
    case 'e':
    case 'E':
        // ui바꾸기
        break;
    case 'q':
    case 'Q':
        ultimate = true;
        silverWolf.w_press = false;
        silverWolf.a_press = false;
        silverWolf.s_press = false;
        silverWolf.d_press = false;
        silverWolf.thisChannel->stop();
        playerChannel = soundManager.Play("silverwolfult", silverWolf.pos, effect_volume);
        break;
    }

}
void game_mode::Keyupboard(unsigned char key, int x, int y) {

    if (game_start || fade || ultimate) return;

    silverWolf.Keyupboard(key, x, y);

    switch (key)
    {
    case 27:
        exit(0);
        break;
    case'g':
    case'G':
        camera_fixed = false;
        //glutSetCursor(GLUT_CURSOR_NONE);
        break;
    case 'e':
    case 'E':
        for (int i = 0; i < balls.size(); i++) {
            if (balls[i].is_nearby) {
                silverWolf.ball_cnt++;
                playerChannel = soundManager.Play("get", silverWolf.pos, effect_volume);
                balls.erase(balls.begin() + i);
                Ball new_ball = Ball(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true);
                new_ball.Init();
                set_ball_in_maze(new_ball);
                balls.push_back(new_ball);
                break;
            }
        }

        if (chest.open_chest(kill_count, 0)) {
            if (account.scores[level] <= 1.0f) {
                account.update_score(level, record_time);
            }
            else {
                if (account.scores[level] > record_time) {
                    account.update_score(level, record_time);
                }
            }

            playerChannel = soundManager.Play("open", silverWolf.pos, effect_volume);
            fade = true;
            camera_fixed = true;
            black_background->color.w = 0.0f;
        }
        break;
    }
}

void game_mode::SpecialKeyboard(int key, int x, int y) {
    if (ultimate)return;
    silverWolf.SpecialKeyboard(key, x, y);
    switch (key)
    {
    case GLUT_KEY_F1:
        if (silverWolf.runSpeed < 6.0f)
            silverWolf.runSpeed = 20.0f;
        else silverWolf.runSpeed = 3.0f;
        is_f1 = !is_f1;
    }
}

void game_mode::SpecialUpKeyboard(int key, int x, int y) {
    if (ultimate)return;
    silverWolf.SpecialUpKeyboard(key, x, y);
}

void game_mode::Mouse(int button, int state, int x, int y) {
    if (ultimate)return;
    if (silverWolf.state != "roll" && silverWolf.state != "jump" && silverWolf.state != "jump_run" && silverWolf.state != "jump_idle") {
        gameCamera.Mouse(button, state, x, y);
    }
    else gameCamera.right_mouth = false;
    silverWolf.Mouse(button, state, x, y);
}

void game_mode::PassiveMotion(int x, int y) {
    if (ultimate)return;
    if (silverWolf.init_success)
        gameCamera.PassiveMotion(x, y, camera_fixed);
}

void  game_mode::Motion(int x, int y) {
    if (ultimate)return;
    gameCamera.Motion(x, y, camera_fixed);
}

// 4. 정리 (종료 시 메모리 해제)
void game_mode::Finish() {
    // 1. trainers 벡터 정리 (동적 할당된 포인터만)
    for (auto trainer : trainers) {
        delete trainer;
    }
    trainers.clear();

    // 2. balls 벡터 정리 (값 타입이므로 clear만)
    balls.clear();

    // 로드된 도로 모델들 삭제
    //for (auto p : roads) {
    //    delete p;
    //}
    //roads.clear();

    // target_model과 chest_model 삭제
    //if (target_model) {
    //    delete target_model;
    //    target_model = nullptr;
    //}
    //
    //if (chest_model) {
    //    delete chest_model;
    //    chest_model = nullptr;
    //}

    // 늑대 모델 삭제
    for (int i = 0; i < 7; ++i) {
        if (silverWolf.silverWolfModel[i]) {
            delete silverWolf.silverWolfModel[i];
            silverWolf.silverWolfModel[i] = nullptr;
        }
    }

    // ui 텍스트 리소스 정리
    if (mission) {
        delete mission;
        mission = nullptr;
    }
    if (black_background) {
        delete black_background;
        black_background = nullptr;
    }
    if (game_ui) {
        delete game_ui;
        game_ui = nullptr;
    }

    // skybox 정리
    if (skybox) {
        delete skybox;
        skybox = nullptr;
    }

    // 셰이더 프로그램 삭제
    glDeleteProgram(shaderProgramStatic);
    glDeleteProgram(shaderProgramAnimated);
    glDeleteProgram(shaderProgramSkybox);
    glDeleteProgram(shaderProgramImage);
    glDeleteProgram(shaderProgramText);
}

void game_mode::Reshape(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    glViewport(0, 0, w, h);
}

void game_mode::OnPause() {
    // 옵션 창 등을 열었을 때 멈춰야 할 로직이 있다면 여기에 작성
}

void game_mode::OnResume() {
    // 옵션 창 닫고 돌아왔을 때 복구할 로직
    glEnable(GL_DEPTH_TEST); // 혹시 다른 씬에서 껐을까봐 다시 켬
}

// ==========================================================
// [Helper Functions] 기존 main.cpp의 함수들을 멤버 함수로 이식
// ==========================================================

void game_mode::loadModels() {
    //미로
    maze.setMaze();
    maze.initmaze();

    //과녁
    target_count = (maze_x+ maze_y) / 2; //과녁 개수 미로 크기에 비례
    target.init(target_count);
    set_target_in_maze();    //미로에 과녁 배치
    kill_count = 0;

    //보물
    chest.init(maze.mazeBlocks[maze_x + 1].reset);

    //트레이너
    /*Trainer* newTrainer = new Trainer();
    newTrainer->Init();
    trainers.push_back(newTrainer);*/

    //은랑
    silverWolf.Init();

    //몬스터볼 ㅇㅅㅇ;
    silverWolf.ball_cnt = 5;
    ball_cnt = maze_x * 5;
    for (int i = 0; i < ball_cnt; i++) {
        Ball new_ball = Ball(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true);
        new_ball.Init();
        set_ball_in_maze(new_ball);
        balls.push_back(new_ball);
    }

}

void game_mode::loadImages() {
    stbi_set_flip_vertically_on_load(true);

    glm::vec2 size1 = glm::vec2((float)winWidth, (float)winHeight);
    glm::vec2 pos1 = glm::vec2((float)winWidth / 2.0f, (float)winHeight / 2.0f);

    mission = new Image(LoadTexture("scene_image/mission.png"), pos1, size1);
    mission->color.w = 1.0f;

    black_background = new Image(LoadTexture("scene_image/black_background.png"), pos1, size1);
    black_background->color.w = 0.5f;

    game_ui = new Image(LoadTexture("scene_image/game_ui.png"), pos1, size1);
    game_ui->color.w = 1.0f;

    e_key_image = new Image(LoadTexture("scene_image/e_key.png"), pos1, size1);
    e_key_image->color.w = 0.8f;

    stbi_set_flip_vertically_on_load(false);
}

void game_mode::loadTexts() {
    glm::mat4 proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

    textUI.Init("font/경기천년제목_Medium.ttf", shaderProgramText, proj);

    //기록용
    record_time = 0.0f;
    if (maze_x == 5) level = 0;
    else if (maze_x == 15) level = 1;
    else if (maze_x == 25) level = 2;
}

void game_mode::setCommonUniforms(GLuint shaderID, const glm::mat4& view, const glm::mat4& proj) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(gameCamera.camPos));

    glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform1f(glGetUniformLocation(shaderID, "ambientStrength"), ambientStrength);

    glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(materialSpecular));
    glUniform1i(glGetUniformLocation(shaderID, "shininess"), shininess);
}

char* game_mode::filetobuf(const char* file) {
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

void game_mode::loadShader(const char* vertPath, const char* fragPath, GLuint& shaderID) {
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

void game_mode::set_target_in_maze() {  // 과녁에서 미로 객체를 가져올 수 없어서 여기서 배치해줌
    uniform_int_distribution<int> rd_x(1, maze_x - 2);
    uniform_int_distribution<int> rd_y(1, maze_y - 2);

    for (int i = 0; i < target.targetBlocks.size();i++) {

        while (1) {
            int rand_x = rd_x(mt), rand_y = rd_y(mt);
            bool rd_flag = false;
            if (maze.maze[rand_y][rand_x].path_wall == WALL || maze.maze[rand_y][rand_x].type == 15 || (rand_y == 1 && rand_x == 1)) continue;
            for (int j = 0; j < i;j++) {
                if (target.targetBlocks[j].modelMatrix == maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix) {
                    rd_flag = true;
                    break;
                }
            }
            if (rd_flag) continue;

            target.targetBlocks[i].modelMatrix = maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix;
            target.targetBlocks[i].modelMatrix = glm::translate(target.targetBlocks[i].modelMatrix, glm::vec3(0.0f, ROAD_SIZE / 7, 0.0f));
            target.targetBlocks[i].reset = glm::vec3(target.targetBlocks[i].modelMatrix[3][0], ROAD_SIZE / 6, target.targetBlocks[i].modelMatrix[3][2]); // 위치 저장
            //cout << rand_y << "," << rand_x << endl;
            break;
        }
    }

}

void game_mode::set_ball_in_maze(Ball& b) {  // 과녁에서 미로 객체를 가져올 수 없어서 여기서 배치해줌
    uniform_int_distribution<int> rd_x(1, maze_x - 2);
    uniform_int_distribution<int> rd_y(1, maze_y - 2);

    uniform_real_distribution<float> rd_in_road(-10.0f, 10.0f);
    bool rd_flag = false;

    do {
        rd_flag = false;
        int rand_x = rd_x(mt), rand_y = rd_y(mt);
        if (maze.maze[rand_y][rand_x].path_wall == WALL || maze.maze[rand_y][rand_x].type == 15 || (rand_y == 1 && rand_x == 1)) {
            rd_flag = true;
            continue;
        }

        glm::vec3 maze_matrix = maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix[3];
        b.current_pos = glm::vec3(maze_matrix.x + rd_in_road(mt), 0.05f, maze_matrix.z + rd_in_road(mt));
        b.update_world_obb();
        for (auto& o : maze.mazeBlocks[(rand_y * maze_x) + rand_x].obstacle_world_obb) {
            if (check_collision(b.ball_obb, o)) {
                rd_flag = true;
                break;
            }
        }
    } while (rd_flag);

}
void game_mode::set_trainer_in_maze(Trainer& t) {  // 과녁에서 미로 객체를 가져올 수 없어서 여기서 배치해줌
    uniform_int_distribution<int> rd_x(1, maze_x - 2);
    uniform_int_distribution<int> rd_y(1, maze_y - 2);

    uniform_real_distribution<float> rd_in_road(-10.0f, 10.0f);
    bool rd_flag = false;

    do {
        rd_flag = false;
        int rand_x = rd_x(mt), rand_y = rd_y(mt);
        if (maze.maze[rand_y][rand_x].path_wall == WALL || maze.maze[rand_y][rand_x].type == 15 || (rand_y == 1 && rand_x == 1)) {
            rd_flag = true;
            continue;
        }

        glm::vec3 maze_matrix = maze.mazeBlocks[(rand_y * maze_x) + rand_x].modelMatrix[3];
        t.pos = glm::vec3(maze_matrix.x + rd_in_road(mt), 0.05f, maze_matrix.z + rd_in_road(mt));
        t.update_world_obb();
        for (auto& o : maze.mazeBlocks[(rand_y * maze_x) + rand_x].obstacle_world_obb) {
            if (check_collision(t.trainer_world_obb, o)) {
                rd_flag = true;
                break;
            }
        }
    } while (rd_flag);

}