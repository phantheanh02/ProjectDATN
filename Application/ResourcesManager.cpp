#include "stdafx.h"
#include <fstream>
#include <algorithm>
#include "ResourcesManager.h"
#include "Globals.h"
#include "SDL_ttf.h"

GLint ParseResourceType(const std::string& type);

void ResourcesManager::Init()
{
	m_init = true;
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		std::cerr << "Audio initialization error: " << Mix_GetError() << "\n";
		m_init = false;
	}
	if (TTF_Init() != 0) 
	{
		std::cerr << "Unable to initialize SDL_ttf: " << TTF_GetError() << "\n";
		m_init = false;
	}
}

void ResourcesManager::CleanUp()
{
	for (auto it = m_modelList.begin(); it != m_modelList.end(); ++it)
	{
		it->second.reset();
	}
	for (auto it = m_shaderList.begin(); it != m_shaderList.end(); ++it)
	{
		it->second.reset();
	}
	for (auto it = m_textureList.begin(); it != m_textureList.end(); ++it)
	{
		auto texId = it->second->GetTextureId();
		glDeleteTextures(1, &texId);
		it->second.reset();
	}
	for (auto it = m_soundList.begin(); it != m_soundList.end(); ++it)
	{
		it->second->FreeSound();
		it->second.reset();
	}
	for (auto& it : m_fontList)
	{
		TTF_CloseFont(it.second);
	}
	m_modelList.clear();
	m_shaderList.clear();
	m_textureList.clear();
	m_soundList.clear();
	m_fontList.clear();

	Mix_CloseAudio();
	TTF_Quit();
}

void ResourcesManager::LoadResources(const std::string& filename)
{
	std::string path = Globals::scenePath + filename;
	std::ifstream resourcesFile(path);
	if (!resourcesFile.is_open())
	{
		std::cerr << "Error opening file " << path << "\n";
		m_init = false;
		return;
	}
	std::string skipStr;
	GLint count, type;
	bool valid = true;
	do
	{
		if (!(resourcesFile >> skipStr))
		{
			break;
		}
		type = ParseResourceType(skipStr);
		switch (type)
		{
		case RT_MODEL:
			LoadModel(resourcesFile);
			break;
		case RT_SHADER:
			LoadShader(resourcesFile);
			break;
		case RT_TEXTURE:
			LoadTexture(resourcesFile);
			break;
		case RT_SOUND:
			LoadSound(resourcesFile);
			break;
		case RT_TTF_FONT:
			LoadFont(resourcesFile);
			break;
		case RT_INVALID:
			valid = false;
			break;
		default:
			break;
		}
	} while (valid);
	if (!valid) std::cout << "Resources file format error, abort loading resources\n";
	resourcesFile.close();
}

std::shared_ptr<Model> ResourcesManager::GetModel(ModelType type)
{
	switch (type)
	{
	case R_RETANGLE_TOPRIGHT:
		return m_modelList[0];
	case R_RETANGLE_CENTER:
		return m_modelList[1];
	case L_RETANGLE_TOPRIGHT:
		return m_modelList[2];
	case L_RETANGLE_CENTER:
		return m_modelList[3];
	default:
		break;
	}

	std::cerr << "ERR: Model " << type << " not found!\n";
	return nullptr;
	return std::shared_ptr<Model>();
}

std::shared_ptr<Shaders> ResourcesManager::GetShader(GLint id)
{
	for (auto it : m_shaderList)
	{
		if (it.second->GetID() == id)
			return it.second;
	}
	std::cerr << "ERR: Shader with id " << id << " not found!\n";
	return nullptr;
}

std::shared_ptr<Texture> ResourcesManager::GetTexture(GLint id)
{
	for (auto it : m_textureList)
	{
		if (it.second->GetID() == id)
			return it.second;
	}
	std::cerr << "ERR: Texture with id " << id << " not found!\n";
	return nullptr;
}

std::shared_ptr<Sound> ResourcesManager::GetSound(GLint id)
{
	auto it = m_soundList.find(id);
	if (it != m_soundList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Sound with id " << id << " not found!\n";
	return nullptr;
}

TTF_Font* ResourcesManager::GetFont(GLint id)
{
	auto it = m_fontList.find(id);
	if (it != m_fontList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Font with id " << id << " not found!\n";
	return nullptr;
}

std::shared_ptr<Shaders> ResourcesManager::GetShader(const char* name)
{
	auto it = m_shaderList.find(name);
	if (it != m_shaderList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Shader with name " << name << " not found!\n";
	return nullptr;
}

std::shared_ptr<Texture> ResourcesManager::GetTexture(const char* name)
{
	auto it = m_textureList.find(name);
	if (it != m_textureList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Texture with name " << name << " not found!\n";
	return nullptr;
}

std::shared_ptr<Model> ResourcesManager::GetModel(GLint id)
{
	auto it = m_modelList.find(id);
	if (it != m_modelList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Model with id " << id << " not found!\n";
	return nullptr;
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

GLint ParseResourceType(const std::string& type)
{
	if (type == "#Models")
		return RT_MODEL;
	if (type == "#Shaders")
		return RT_SHADER;
	if (type == "#2D_Textures")
		return RT_TEXTURE;
	if (type == "#Sounds")
		return RT_SOUND;
	if (type == "#Fonts")
		return RT_TTF_FONT;
	return RT_INVALID;
}

void ResourcesManager::LoadModel(std::ifstream& filePtr)
{
	std::string skipStr, filename;
	GLint id;
	filePtr >> skipStr;
	while (skipStr != "$")
	{
		filePtr >> id >> skipStr >> filename >> skipStr;
		filename.erase(std::remove_if(filename.begin(), filename.end(), [](char c) {return c == '\"'; }), filename.end());
		std::string file = Globals::modelPath + filename;
		std::cout << "Load model: " << file << "\n";
		std::shared_ptr<Model> model = std::make_shared<Model>();
		model->LoadModel(file);
		model->SetID(id);
		m_modelList.insert(std::make_pair(id, model));
	}
}

void ResourcesManager::LoadShader(std::ifstream& filePtr)
{
	std::string skipStr, filename, vs, fs;
	GLint id;
	filePtr >> skipStr;
	while (skipStr != "$")
	{
		filePtr  >> id >> skipStr >> filename >> skipStr;
		filename.erase(std::remove_if(filename.begin(), filename.end(), [](char c) {return c == '\"'; }), filename.end());
		std::cout << "Load shader: " << filename << "\n";
		vs = Globals::shaderPath + filename + ".vert";
		fs = Globals::shaderPath + filename + ".frag";
		std::cout << "Load vert shader: " << vs << "\n";
		std::cout << "Load frag shader: " << fs << "\n";

		auto shader = std::make_shared<Shaders>();
		shader->Init(vs.c_str(), fs.c_str());
		shader->SetID(id);
		m_shaderList.insert(std::make_pair(filename, shader));
	}
}

void ResourcesManager::LoadTexture(std::ifstream& filePtr)
{
	std::string skipStr, filename, wrap, filter;
	GLint id;
	filePtr >> skipStr;
	while (skipStr != "$")
	{
		filePtr >> id >> skipStr >> filename >> skipStr;
		filename.erase(std::remove_if(filename.begin(), filename.end(), [](char c) {return c == '\"'; }), filename.end());
		std::string file = Globals::texturePath + filename;
		std::cout << "Load texture: " << filename << "\n";

		auto texture = std::make_shared<Texture>();
		texture->LoadTexture(file);
		texture->SetID(id);
		texture->SetWrap(GL_REPEAT);
		texture->SetFilter(GL_NEAREST);

		m_textureList.insert(std::make_pair(filename, texture));
	}
}

void ResourcesManager::LoadSound(std::ifstream& filePtr)
{
	std::string skipStr, filename;
	GLint id;
	filePtr >> skipStr;
	while (skipStr != "$")
	{
		filePtr  >> id >> skipStr >> filename >> skipStr;
		filename.erase(std::remove_if(filename.begin(), filename.end(), [](char c) {return c == '\"'; }), filename.end());
		filename = Globals::soundPath + filename;
		std::cout << "Load sound: " << filename << "\n";
		auto sound = std::make_shared<Sound>();
		sound->LoadSound(filename);
		m_soundList.insert(std::make_pair(id, sound));
	}
}

void ResourcesManager::LoadFont(std::ifstream& filePtr)
{
	std::string skipStr, filename;
	GLint id, size;
	filePtr >> skipStr;
	while (skipStr != "$")
	{
		filePtr >> id >> skipStr >> filename >> skipStr >> size >> skipStr;
		filename.erase(std::remove_if(filename.begin(), filename.end(), [](char c) {return c == '\"'; }), filename.end());
		filename = Globals::fontPath + filename;
		std::cout << "Load font: " << filename << "\n";
		auto font = TTF_OpenFont(filename.c_str(), size);
		if (!font)
		{
			std::cerr << "Error opening font: " << TTF_GetError() << "\n";
			return;
		}
		m_fontList.insert(std::make_pair(id, font));
	}
}
