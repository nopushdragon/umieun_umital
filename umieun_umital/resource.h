#pragma once
#include "animated_model.h"
#include "static_model.h"
#include "sound_manager.h"

extern NewModel* s[7];

extern std::vector<StaticModel*> roads;
extern StaticModel* target_model;
extern StaticModel* chest_model;
extern StaticModel* ball_model;
extern NewModel* mei_model[3];
extern NewModel* press_model[3];
extern NewModel* archie_model[3];
extern NewModel* serena_model[3];
extern Sound_Manager soundManager;


void Resource_Init();
