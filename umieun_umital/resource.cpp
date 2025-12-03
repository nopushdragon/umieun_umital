#include "resource.h"

NewModel* s[7];
std::vector<StaticModel*> roads;
StaticModel* target_model;
StaticModel* chest_model;
StaticModel* ball_model;
NewModel* mei_model[3];
NewModel* press_model[3];
NewModel* archie_model[3];
NewModel* serena_model[3];


void Resource_Init() {
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

	//캐릭터 모델들
	press_model[0] = new NewModel("press/Standing Walk Forward.fbx");
	press_model[0]->state = "walk";
    press_model[1] = new NewModel("press/Head Spinning.fbx");
    press_model[1]->state = "tracking";
    press_model[2] = new NewModel("press/Run To Dive.fbx");
	press_model[2]->state = "die";
	mei_model[0] = new NewModel("mei/Jogging.fbx");
	mei_model[0]->state = "walk";
    mei_model[1] = new NewModel("mei/Breakdance 1990.fbx");
	mei_model[1]->state = "tracking";
    mei_model[2] = new NewModel("mei/Knocked Out.fbx");
    mei_model[2]->state = "die";
	archie_model[0] = new NewModel("Archie/Walking.fbx");
	archie_model[0]->state = "walk";
    archie_model[1] = new NewModel("Archie/Flair.fbx");
	archie_model[1]->state = "tracking";
    archie_model[2] = new NewModel("Archie/Falling Down.fbx");
	archie_model[2]->state = "die";
	serena_model[0] = new NewModel("Serena/Walking.fbx");
	serena_model[0]->state = "walk";
    serena_model[1] = new NewModel("Serena/Treadmill Running.fbx");
	serena_model[1]->state = "tracking";
    serena_model[2] = new NewModel("Serena/Turn To Knocked Unconscious.fbx");
	serena_model[2]->state = "die";

       
}