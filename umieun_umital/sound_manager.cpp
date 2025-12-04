#include "sound_manager.h"

Sound_Manager::Sound_Manager()
{
	Sound_Manager::Init();
}

void Sound_Manager::Init()
{
	FMOD::System_Create(&system);
	system->init(256, FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_3D_RIGHTHANDED, 0);
	system->set3DSettings(1.0f, 1.0f, 1.0f);
}

void Sound_Manager::Update()
{
	system->update();
}

void Sound_Manager::LoadSound(const string& name, const string& path, bool is3D, bool loop)
{
	FMOD_MODE mode = FMOD_DEFAULT;
	if (is3D) mode |= FMOD_3D;
	else mode |= FMOD_2D;
	if (loop) mode |= FMOD_LOOP_NORMAL;
	else mode |= FMOD_LOOP_OFF;
	Sound* sound = nullptr;
	system->createSound(path.c_str(), mode, 0, &sound);
	
	if (sound) {
		if (is3D) sound->set3DMinMaxDistance(1.0f, 1000.0f);
		soundMap[name] = sound;
	}
	else {
		std::cout << "[FMOD Error] 사운드 로드 실패: " << name << std::endl;
	}
}

Channel* Sound_Manager::Play(const string& name, glm::vec3 pos, float volume)
{
	Channel* channel = nullptr;
	system->playSound(soundMap[name], 0, false, &channel);
	if (channel) {
		FMOD_VECTOR fmodPos = ToFmod(pos);
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
		channel->set3DAttributes(&fmodPos, &vel);
		channel->setVolume(volume);
	}
	return channel;
	
}



void Sound_Manager::SetListenerAttributes(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up, glm::vec3 vel)
{
	FMOD_VECTOR fmodPos = ToFmod(pos);
	FMOD_VECTOR fmodForward = ToFmod(forward);
	FMOD_VECTOR fmodUp = ToFmod(up);
	FMOD_VECTOR fmodVel = ToFmod(vel);
	system->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
}

void Sound_Manager::UpdateChannelPosition(Channel* channel, const glm::vec3& pos, glm::vec3 vel)
{
	if (channel) {
		FMOD_VECTOR fmodPos = ToFmod(pos);
		FMOD_VECTOR fmodVel = ToFmod(vel);
		channel->set3DAttributes(&fmodPos, &fmodVel);
	}
}

Sound_Manager::~Sound_Manager()
{
	for (auto& pair : soundMap) {
		if (pair.second) {
			pair.second->release();
		}
	}
	if (system) {
		system->close();
		system->release();
	}
}