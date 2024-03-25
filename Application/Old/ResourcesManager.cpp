#include "stdafx.h"
#include "ResourcesManager.h"

void ResourcesManager::Init()
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cerr << "SDL initialization error: " << SDL_GetError() << "\n";
		m_init = false;
	}
	else if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "Audio initialization error: " << Mix_GetError() << "\n";
		m_init = false;
	}
	else m_init = true;
}

std::shared_ptr<Model> ResourcesManager::getModel(const std::string& filename)
{
	auto it = m_modelList.find(filename);
	if (it != m_modelList.end())
	{
		return it->second;
	}
	auto model = std::make_shared<Model>();
	model->LoadModel(filename);
	m_modelList.insert(std::make_pair(filename, model));
	return model;
}

std::shared_ptr<Shaders> ResourcesManager::getShader(const std::string& filename)
{
	auto pathVS = filename + ".vs";
	auto pathFS = filename + ".fs";
	auto it = m_shaderList.find(filename);
	if (it != m_shaderList.end())
	{
		return it->second;
	}
	auto shader = std::make_shared<Shaders>();
	shader->Init(pathVS.c_str(), pathFS.c_str());
	m_shaderList.insert(std::make_pair(filename, shader));
	return shader;
}

std::shared_ptr<Texture> ResourcesManager::getTexture(const std::string& filename)
{
	auto it = m_textureList.find(filename);
	if (it != m_textureList.end())
	{
		return it->second;
	}
	auto texture = std::make_shared<Texture>();
	texture->LoadTexture(filename);
	m_textureList.insert(std::make_pair(filename, texture));
	return texture;
}

std::shared_ptr<Sound> ResourcesManager::getSound(const std::string& filename)
{
	auto it = m_soundList.find(filename);
	if (it != m_soundList.end())
	{
		return it->second;
	}
	auto sound = std::make_shared<Sound>();
	sound->LoadSound(filename);
	m_soundList.insert(std::make_pair(filename, sound));
	return sound;
}

void ResourcesManager::SetMasterVolume(int volume)
{
	volume = volume < 0 ? 0 : volume;
	volume = volume > 100 ? 100 : volume;
	GLfloat value = (GLfloat)volume / 100 * 128;
	Mix_Volume(-1, (GLint)value);
}

void ResourcesManager::PauseAllSounds()
{
	Mix_Pause(-1);
}

void ResourcesManager::StopAllSounds()
{
	Mix_HaltChannel(-1);
}

void ResourcesManager::ResumeAllSounds()
{
	Mix_Resume(-1);
}

void ResourcesManager::CleanUp()
{
	for (auto it = m_modelList.begin(); it != m_modelList.end(); ++it)
	{
		auto ibo = it->second->getIBOid();
		auto vbo = it->second->getVBOid();
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		it->second.reset();
	}
	for (auto it = m_shaderList.begin(); it != m_shaderList.end(); ++it)
	{
		glDeleteShader(it->second->fragmentShader);
		glDeleteShader(it->second->vertexShader);
		it->second.reset();
	}
	for (auto it = m_textureList.begin(); it != m_textureList.end(); ++it)
	{
		auto texId = it->second->getTextureId();
		glDeleteTextures(1, &texId);
		it->second.reset();
	}
	for (auto it = m_soundList.begin(); it != m_soundList.end(); ++it)
	{
		it->second.reset();
	}
	m_modelList.clear();
	m_shaderList.clear();
	m_textureList.clear();
	m_soundList.clear();

	Mix_CloseAudio();
	SDL_Quit();
}
