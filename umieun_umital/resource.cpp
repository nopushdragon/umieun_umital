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

Sound_Manager soundManager;

FMOD::Channel* bgmChannel = nullptr;
FMOD::Channel* playerChannel = nullptr;
FMOD::Channel* meiChannel = nullptr;
FMOD::Channel* ArchieChannel = nullptr;
FMOD::Channel* SerenaChannel = nullptr;
FMOD::Channel* pressChannel = nullptr;
float bgm_volume = 0.4f;
float effect_volume = 0.3f;


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


    //브금들 입니다
	soundManager.LoadSound("logo", "sound/bgm/logo.mp3",false,false);
    soundManager.LoadSound("title", "sound/bgm/title.mp3", false, true);
    soundManager.LoadSound("main0", "sound/bgm/main0.mp3", false, false);
    soundManager.LoadSound("main1", "sound/bgm/main1.mp3", false, false);
    soundManager.LoadSound("main2", "sound/bgm/main2.mp3", false, false);
    soundManager.LoadSound("main3", "sound/bgm/main3.mp3", false, false);
    soundManager.LoadSound("main4", "sound/bgm/main4.mp3", false, false);
    soundManager.LoadSound("silverwolf0", "sound/silver wolf/silverwolf0.wav", false, false);
    soundManager.LoadSound("silverwolf1", "sound/silver wolf/silverwolf1.wav", false, false);
    soundManager.LoadSound("silverwolf2", "sound/silver wolf/silverwolf2.wav", false, false);
    soundManager.LoadSound("attack", "sound/silver wolf/attack.mp3", false, false);
    soundManager.LoadSound("roll", "sound/silver wolf/dash.mp3", false, false);
    soundManager.LoadSound("walk", "sound/silver wolf/walk.wav", false, false);
    soundManager.LoadSound("run", "sound/silver wolf/run.wav", false, false);
    soundManager.LoadSound("game_over", "sound/silver wolf/game_over.mp3", false, false);
    soundManager.LoadSound("00", "sound/mei/mei0.ogg", true, false);
    soundManager.LoadSound("01", "sound/mei/mei1.ogg", true, false);
    soundManager.LoadSound("02", "sound/mei/mei2.ogg", true, false);
    soundManager.LoadSound("20", "sound/Archie/Archie0.ogg", true, false);
    soundManager.LoadSound("21", "sound/Archie/Archie1.ogg", true, false);
    soundManager.LoadSound("22", "sound/Archie/Archie2.ogg", true, false);
    soundManager.LoadSound("10", "sound/Serena/Serena0.ogg", true, false);
    soundManager.LoadSound("11", "sound/Serena/Serena1.ogg", true, false);
    soundManager.LoadSound("12", "sound/Serena/Serena2.ogg", true, false);
    soundManager.LoadSound("30", "sound/press/press0.wav", true, false);
    soundManager.LoadSound("31", "sound/press/press1.wav", true, false);
    soundManager.LoadSound("32", "sound/press/press2.wav", true, false);
    soundManager.LoadSound("catch", "sound/ball/catch.mp3", true, false);
    soundManager.LoadSound("throw", "sound/ball/throw.mp3", true, false);
    soundManager.LoadSound("cry", "sound/pokemon/drifloon.mp3", true, false);
 




       
}