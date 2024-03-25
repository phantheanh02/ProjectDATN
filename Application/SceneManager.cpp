#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SceneManager.h"
#include "ResourcesManager.h"
#include "Globals.h"

int ParseElementType(const std::string& type);

void SceneManager::Init()
{
	m_init = true;
	// dynamic camera for objects
	std::shared_ptr<Camera> dynamicCamera = std::make_shared<Camera>(0);

	// static camera for UI
	std::shared_ptr<Camera> staticCamera = std::make_shared<Camera>(1);

	m_cameraList.insert(std::make_pair(0, dynamicCamera));
	m_cameraList.insert(std::make_pair(1, staticCamera));

	m_soundVolume = 100;
	m_lastNonZeroSoundVolume = 100;
}

void SceneManager::CleanUp()
{
	for (auto& it : m_cameraList)
	{
		it.second.reset();
	}
	for (auto& it : m_objectList)
	{
		it.second.reset();
	}
	m_cameraList.clear();
	m_objectList.clear();
}

void SceneManager::LoadElements(const std::string& filename)
{
	std::string path = Globals::scenePath + filename;
	std::ifstream fscene(path);
	if (!fscene.is_open())
	{
		std::cerr << "Error opening file " << path << "\n";
		m_init = false;
		return;
	}
	std::string typeStr;
	GLint count = 0, type;
	bool valid = true;
	do
	{
		if (!(fscene >> typeStr >> count))
		{
			break;
		}
		type = ParseElementType(typeStr);
		switch (type)
		{
		case ET_CAMERA:
			LoadCamera(count, fscene);
			break;
		case ET_OBJECT:
			LoadObject(count, fscene);
			break;
		case ET_ENEMY:
			LoadEnemies(count, fscene);
			break;
		case ET_BULLET:
			LoadBullet(count, fscene);
			break;
		default:
			std::cerr << "Scene file format error, abort loading elements";
			valid = false;
			m_init = false;
			break;
		}
	} while (valid);
	fscene.close();
}

std::shared_ptr<Camera> SceneManager::GetCamera(GLint camera_id)
{
	auto it = m_cameraList.find(camera_id);
	if (it != m_cameraList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Camera with id " << camera_id << " not found!\n";
	return nullptr;
}

std::shared_ptr<Sprite2D> SceneManager::GetObject(GLint object_id)
{
	auto it = m_objectList.find(object_id);
	if (it != m_objectList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Object with id " << object_id << " not found!\n";
	return nullptr;
}

BoxEnemy SceneManager::GetBoxEnemy(GLint enemy_id)
{
	for (auto it : m_enemiesList)
	{
		if (it.first == enemy_id)
		{
			return it.second;
		}
	}
	std::cerr << "ERR: Enemy with id " << enemy_id << " not found!\n";
	return BoxEnemy();
}

BoxBullet SceneManager::GetBoxBullet(GLint bullet_id)
{
	for (auto it : m_bulletList)
	{
		if (it.first == bullet_id)
		{
			return it.second;
		}
	}
	std::cerr << "ERR: Enemy with id " << bullet_id << " not found!\n";
	return BoxBullet();
}

void SceneManager::LoadObject(int count, std::ifstream& file)
{
	Vector3 pos, rt, sc;
	GLint id, modelId, textureId, shaderId;
	std::string skipStr;
	for (int i = 0; i < count; ++i)
	{
		file >> skipStr >> id >> skipStr >> modelId >> skipStr >> textureId >> skipStr >> shaderId;
		file >> skipStr >> pos.x >> pos.y >> pos.z;
		file >> skipStr >> rt.x >> rt.y >> rt.z;
		file >> skipStr >> sc.x >> sc.y >> sc.z;
		auto model = ResourcesManager::GetInstance()->GetModel(modelId);
		auto shader = ResourcesManager::GetInstance()->GetShader(shaderId);
		auto texture = ResourcesManager::GetInstance()->GetTexture(textureId);
		auto obj = std::make_shared<Sprite2D>(id, model, shader, texture);
		obj->SetPos(pos);
		obj->SetScale(sc);
		obj->SetRotation(rt);
		m_objectList.insert(std::make_pair(id, obj));
	}
}

void SceneManager::LoadCamera(int count, std::ifstream& file)
{
	std::string skipStr;
	Vector3 pos;
	GLint id;
	GLfloat nr, fr, fov, mspeed, rspeed;
	for (int i = 0; i < count; ++i)
	{
		file >> skipStr >> id;
		file >> skipStr >> pos.x >> pos.y >> pos.z;
		file >> skipStr >> nr >> skipStr >> fr;
		file >> skipStr >> fov;
		file >> skipStr >> mspeed;
		file >> skipStr >> rspeed;
		auto camera = std::make_shared<Camera>(id, pos, nr, fr, fov, rspeed, mspeed);
		m_cameraList.insert(std::make_pair(id, camera));
	}
}

void SceneManager::LoadEnemies(int count, std::ifstream& file)
{
	std::string skipStr;
	BoxEnemy box;
	file >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;

	for (int i = 0; i < count; i++)
	{
		BoxEnemy box;
		file >> box.id >> box.imgSize.x >> box.imgSize.y >> box.boxSize.x >> box.boxSize.y;
		m_enemiesList.insert(std::make_pair(box.id, box));
	}
}

void SceneManager::LoadBullet(int count, std::ifstream& file)
{
	std::string skipStr;
	BoxBullet box;
	file >> skipStr >> skipStr >> skipStr;

	for (int i = 0; i < count; i++)
	{
		BoxBullet box;
		file >> box.id >> box.imgSize.x >> box.imgSize.y;
		m_bulletList.insert(std::make_pair(box.id, box));
	}
}

int ParseElementType(const std::string& type)
{
	if (type == "#Objects:")
		return ET_OBJECT;
	if (type == "#Cameras:")
		return ET_CAMERA;
	if (type == "#Enemies:")
		return ET_ENEMY;
	if (type == "#Bullets:")
		return ET_BULLET;
	return ET_INVALID;
}
