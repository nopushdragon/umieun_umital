#include "resource.h"

NewModel* s[7];
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

	ball_model = new StaticModel("ball/Pokeball.obj");
    ball_model->set_ball_obb();
}