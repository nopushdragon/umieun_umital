#include "resource.h"

NewModel* s[7];
std::vector<StaticModel*> roads;
StaticModel* target_model;
StaticModel* chest_model;
StaticModel* ball_model;
void Resource_Init() {
   /* s[0] = new NewModel("silver_wolf/Idle.fbx");
    s[0]->state = "idle";
    s[1] = new NewModel("silver_wolf/Walk.fbx");
    s[1]->state = "walk";
    s[2] = new NewModel("silver_wolf/Stop Walking.fbx");
    s[2]->state = "stop_walking";
    s[3] = new NewModel("silver_wolf/Running.fbx");
    s[3]->state = "running";
    s[4] = new NewModel("silver_wolf/Run To Stop.fbx");
    s[4]->state = "run_to_stop";
    s[5] = new NewModel("silver_wolf/Throw.fbx");
    s[5]->state = "throw";
    s[6] = new NewModel("silver_wolf/Stand To Roll.fbx");
    s[6]->state = "stand_to_roll";*/

    // 미로
    roads.push_back(new StaticModel("road/road0.obj"));   // 0동
    roads.push_back(new StaticModel("road/road1.obj"));   // 1서 
    roads.push_back(new StaticModel("road/road2.obj"));   // 2남 
    roads.push_back(new StaticModel("road/road3.obj"));   // 3북
    roads.push_back(new StaticModel("road/road4.obj"));   // 4ㅡ
    roads.push_back(new StaticModel("road/road5.obj"));   // 5ㅣ
    roads.push_back(new StaticModel("road/road6.obj"));   // 6┌
    roads.push_back(new StaticModel("road/road7.obj"));   // 7┐
    roads.push_back(new StaticModel("road/road8.obj"));   // 8└
    roads.push_back(new StaticModel("road/road9.obj"));   // 9┘
    roads.push_back(new StaticModel("road/road10.obj"));  // 10ㅏ
    roads.push_back(new StaticModel("road/road11.obj"));  // 11ㅓ
    roads.push_back(new StaticModel("road/road12.obj"));  // 12ㅜ
    roads.push_back(new StaticModel("road/road13.obj"));  // 13ㅗ
    roads.push_back(new StaticModel("road/road14.obj"));  // 14+
    roads.push_back(new StaticModel("road/road15.obj"));  // 15x
    for (int i = 0; i < roads.size(); i++) {
        roads[i]->set_maze_obb(i);
    }

    //과녁
    target_model = new StaticModel("target/baudrive.obj");
    target_model->set_target_obb();

    //보물
    chest_model = new StaticModel("chest/chest.obj");
    chest_model->set_chest_obb();

	//볼
	ball_model = new StaticModel("ball/Pokeball.obj");
    ball_model->set_ball_obb();
}