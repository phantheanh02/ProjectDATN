#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include "Singleton.h"
#include "Model.h"
#include "Shaders.h"
#include "Texture.h"
#include "Camera.h"
#include "Sound.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

enum ResourceType
{
	RT_MODEL,
	RT_SHADER,
	RT_TEXTURE,
	RT_SOUND,
	RT_TTF_FONT,
	RT_INVALID = -1
};

class ResourcesManager : public SingletonDclp<ResourcesManager>
{
public:
	// APIs
	void Init();
	void CleanUp();
	void LoadResources(const std::string& filename);

	std::shared_ptr<Model>		GetModel(ModelType type);
	std::shared_ptr<Model>		GetModel(GLint id);
	std::shared_ptr<Shaders>	GetShader(GLint id);
	std::shared_ptr<Texture>	GetTexture(GLint id);
	std::shared_ptr<Sound>		GetSound(GLint id);
	TTF_Font*					GetFont(GLint id);

	std::shared_ptr<Shaders>	GetShader(const char* name);
	std::shared_ptr<Texture>	GetTexture(const char* name);
	void SetMasterVolume(int volume);
	void PauseAllSounds();
	void StopAllSounds();
	void ResumeAllSounds();
	
	bool m_init;
private:
	std::unordered_map<GLint, std::shared_ptr<Model>>			m_modelList;
	std::unordered_map<std::string, std::shared_ptr<Shaders>>	m_shaderList;
	std::unordered_map<std::string, std::shared_ptr<Texture>>	m_textureList;
	std::unordered_map<GLint, std::shared_ptr<Sound>>			m_soundList;
	std::unordered_map<GLint, TTF_Font*>						m_fontList;

	// Utilities
	void LoadModel(std::ifstream& filePtr);
	void LoadShader(std::ifstream& filePtr);
	void LoadTexture(std::ifstream& filePtr);
	void LoadSound(std::ifstream& filePtr);
	void LoadFont(std::ifstream& filePtr);
};