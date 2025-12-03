#pragma once
#include "headers.h"

class Trainer;

class TrainerState
{
public:
    virtual ~TrainerState() {}

    virtual void Enter(Trainer* trainer) = 0;

    virtual void Update(Trainer* trainer, float dt) = 0;

    virtual void Draw(Trainer* trainer, GLuint shaderID,float deltatime) = 0;

    virtual void Exit(Trainer* trainer) = 0;
};