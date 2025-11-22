#include "silver_wolf.h"

silver_wolf::silver_wolf()
{
   /* silverWolfModel[0] = new NewModel("silver_wolf/Idle.fbx");
    silverWolfModel[0]->state = "idle";
    silverWolfModel[1] = new NewModel("silver_wolf/Walk.fbx");
    silverWolfModel[1]->state = "walk";
    silverWolfModel[2] = new NewModel("silver_wolf/Stop Walking.fbx");
    silverWolfModel[2]->state = "stop_walking";
    silverWolfModel[3] = new NewModel("silver_wolf/Running.fbx");
    silverWolfModel[3]->state = "running";
    silverWolfModel[4] = new NewModel("silver_wolf/Run To Stop.fbx");
    silverWolfModel[4]->state = "run_to_stop";
    silverWolfModel[5] = new NewModel("silver_wolf/Throw.fbx");
    silverWolfModel[5]->state = "throw";
    silverWolfModel[6] = new NewModel("silver_wolf/Stand To Roll.fbx");
    silverWolfModel[6]->state = "stand_to_roll";*/

}

void silver_wolf::Draw(GLuint shaderID, float currentTime=0.0f) {

	for (int i = 0; i < silver_wolf_fbx_size; i++) {
		if (silverWolfModel[i]->state == state) {
			silverWolfModel[i]->Draw(shaderID, currentTime);
		}
	}

}

silver_wolf::~silver_wolf()
{
	for (int i = 0; i < silver_wolf_fbx_size; i++)
	{
		delete& silverWolfModel[i];
	}
}