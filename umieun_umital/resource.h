#pragma once
#include "animated_model.h"
#include "static_model.h"

extern NewModel* s[7];

extern std::vector<StaticModel*> roads;
extern StaticModel* target_model;
extern StaticModel* chest_model;
extern StaticModel* ball_model;


void Resource_Init();
