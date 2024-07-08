#pragma once
#include <string>
#include "SDL_mixer.h"
#include "../Utilities/utilities.h"

class Sound
{
public:
	Sound();
	~Sound();
	int LoadSound(const std::string& filename);
	void FreeSound();
	void Play(int timesLoop = false);
	void Pause();
	void Resume();
	void Stop();
	void SetVolume(GLint volume);
	GLint GetCurrentVolume() const { return m_currentVolume; };

private:
	Mix_Chunk* m_sound;
	GLint m_currentVolume;
	GLint m_lastPlayingChannel;
};