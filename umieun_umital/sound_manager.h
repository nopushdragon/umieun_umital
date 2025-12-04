#pragma once
#include "headers.h"

using namespace FMOD;

inline FMOD_VECTOR ToFmod(const glm::vec3& v) { return { v.x, v.y, v.z }; }


class Sound_Manager
{
private:
	System* system = nullptr;
	map<string, Sound*> soundMap;
public:
	Sound_Manager();
	~Sound_Manager();
	void Init();
	void Update();

	void LoadSound(const string& name, const string& path,bool is3D=true, bool loop = false);
	Channel* Play(const string& name, glm::vec3 pos = glm::vec3(0.0f));

	void SetListenerAttributes(const glm::vec3& pos,  const glm::vec3& forward, const glm::vec3& up,  glm::vec3 vel= glm::vec3(0.0f));

	void UpdateChannelPosition(Channel* channel, const glm::vec3& pos, glm::vec3 vel = glm::vec3(0.0f));

};

