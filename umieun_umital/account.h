#pragma once
#include "headers.h"

class Account
{
private:
public:
	float easy_time;
	float normal_time;
	float hard_time;
	float scores[3];

	Account();
	string best_score(int level);
	void update_score(int level, float time);
};

extern Account account;

class account
{
};

