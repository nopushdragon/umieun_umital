#include "silver_wolf.h"

silver_wolf::silver_wolf()
{
	


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