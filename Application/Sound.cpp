#include "stdafx.h"
#include <iostream>
#include "Sound.h"

Sound::Sound()
{
	m_currentVolume = 100;
	m_lastPlayingChannel = -1;
	m_sound = nullptr;
}

Sound::~Sound()
{
	Mix_FreeChunk(m_sound);
	m_sound = nullptr;
}

int Sound::LoadSound(const std::string& filename)
{
	m_sound = Mix_LoadWAV(filename.c_str());
	if (!m_sound)
	{
		return -1;
	}
	return 0;
}

void Sound::FreeSound()
{
	/*Mix_FreeChunk(m_sound);
	m_sound = nullptr;*/
}

void Sound::Play(int timesLoop)
{
	m_lastPlayingChannel = Mix_PlayChannel(-1, m_sound, timesLoop);
}

void Sound::Pause()
{
	Mix_PauseMusic();
	Mix_Pause(m_lastPlayingChannel);
}

void Sound::Resume()
{
	Mix_ResumeMusic();
	Mix_Resume(m_lastPlayingChannel);
}

void Sound::Stop()
{
	int error = Mix_HaltChannel(m_lastPlayingChannel);
	if (error)
	{
		std::cerr << "Error while stopping sound: " << SDL_GetError() << "\n";
	}
}

void Sound::SetVolume(GLint volume)
{
	m_currentVolume = volume < 0 ? 0 : volume;
	m_currentVolume = volume > 100 ? 100 : m_currentVolume;
	GLfloat volumeValue = (GLfloat)m_currentVolume / 100 * 128;
	Mix_VolumeChunk(m_sound, (GLint)volumeValue);
	Mix_VolumeMusic((GLint)volumeValue);
}
