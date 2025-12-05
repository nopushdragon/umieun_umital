#pragma once
#include "animated_model.h"
#include "static_model.h"
#include "sound_manager.h"
#include "image.h"
#define silverWolfult_count 103

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
extern FMOD::Channel* bgmChannel;
extern FMOD::Channel* playerChannel;
extern FMOD::Channel* meiChannel;
extern FMOD::Channel* ArchieChannel;
extern FMOD::Channel* SerenaChannel;
extern FMOD::Channel* pressChannel;
extern Image* silverWolfult[silverWolfult_count];
extern float bgm_volume;
extern float effect_volume;
void Resource_Init();
