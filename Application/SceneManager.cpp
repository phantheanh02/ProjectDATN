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

	m_cameraList.insert(std::make_pair(CameraType::DYNAMIC_CAMERA, dynamicCamera));
	m_cameraList.insert(std::make_pair(CameraType::STATIC_CAMERA, staticCamera));

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
	GLint type;
	bool valid = true;
	do
	{
		if (!(fscene >> typeStr))
		{
			break;
		}
		type = ParseElementType(typeStr);
		switch (type)
		{
		case ET_CAMERA:
			LoadCamera(fscene);
			break;
		case ET_OBJECT:
			LoadObject(fscene);
			break;
		case ET_ENEMY:
			LoadEnemies(fscene);
			break;
		case ET_BULLET:
			LoadBullet(fscene);
			break;
		case ET_MAP:
			LoadMap(fscene);
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


std::shared_ptr<Camera> SceneManager::GetCamera(CameraType camera)
{
	switch (camera)
	{
	case STATIC_CAMERA:
		return m_cameraList[STATIC_CAMERA];
	case DYNAMIC_CAMERA:
		return m_cameraList[DYNAMIC_CAMERA];
	default:
		return nullptr;
	}
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

MapInfo SceneManager::GetMapInfo(GLint map_id)
{
	for (auto it : m_mapList)
	{
		if (it.id == map_id)
		{
			return it;
		}
	}
	std::cerr << "ERR: Map with id " << map_id << " not found!\n";
	return MapInfo();
}

MapInfo SceneManager::GetCurrentMapInfo()
{
	return m_currentMap;
}

void SceneManager::SetCurrentMap(int maptype)
{
	switch (maptype)
	{
	case 0: // MT_BAREN
		m_currentMap = GetMapInfo(1);
		break;
	case 1: // MT_ICE
		m_currentMap = GetMapInfo(2);
		break;
	case 2: // MT_LAVA
		m_currentMap = GetMapInfo(3);
		break;
	case 3: // MT_BLACK_HOLE
		m_currentMap = GetMapInfo(4);
		break;
	case 4: // MT_TERRAN
		m_currentMap = GetMapInfo(4);
		break;
	default:
		break;
	}
}

void SceneManager::LoadObject(std::ifstream& file)
{
	Vector3 pos, rt, sc;
	GLint id, modelId, textureId, shaderId;
	std::string skipStr;
	file >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	while (skipStr != "$")
	{
		file >> skipStr;
		//file >> skipStr >> id >> skipStr >> modelId >> skipStr >> textureId >> skipStr >> shaderId;
		//file >> skipStr >> pos.x >> pos.y >> pos.z;
		//file >> skipStr >> rt.x >> rt.y >> rt.z;
		//file >> skipStr >> sc.x >> sc.y >> sc.z;
		//auto model = ResourcesManager::GetInstance()->GetModel(modelId);
		//auto shader = ResourcesManager::GetInstance()->GetShader(shaderId);
		//auto texture = ResourcesManager::GetInstance()->GetTexture(textureId);
		//auto obj = std::make_shared<Sprite2D>(id, model, shader, texture);
		//obj->SetPos(pos);
		//obj->SetScale(sc);
		//obj->SetRotation(rt);
		//m_objectList.insert(std::make_pair(id, obj));
	}
}

void SceneManager::LoadCamera(std::ifstream& file)
{
	std::string skipStr;
	Vector3 pos;
	GLint id;
	GLfloat nr, fr, fov, mspeed, rspeed;
	file >> skipStr;
	while (skipStr != "$")
	{
		file >> skipStr;
		//file >> skipStr >> id;
		//file >> skipStr >> pos.x >> pos.y >> pos.z;
		//file >> skipStr >> nr >> skipStr >> fr;
		//file >> skipStr >> fov;
		//file >> skipStr >> mspeed;
		//file >> skipStr >> rspeed;
		//auto camera = std::make_shared<Camera>(id, pos, nr, fr, fov, rspeed, mspeed);
		//m_cameraList.insert(std::make_pair(id, camera));
	}
}

void SceneManager::LoadEnemies(std::ifstream& file)
{
	std::string skipStr;
	file >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	file >> skipStr;
	while (skipStr != "$")
	{
		BoxEnemy box;
		box.id = std::stoi(skipStr);
		file >> box.imgSize.x >> box.imgSize.y >> box.boxSize.x >> box.boxSize.y >> skipStr;
		m_enemiesList.insert(std::make_pair(box.id, box));
	}
}

void SceneManager::LoadBullet(std::ifstream& file)
{
	std::string skipStr;
	BoxBullet box;
	file >> skipStr >> skipStr >> skipStr;
	file >> skipStr;
	while (skipStr != "$")
	{
		BoxBullet box;
		box.id = std::stoi(skipStr);
		file >> box.imgSize.x >> box.imgSize.y >> skipStr;
		m_bulletList.insert(std::make_pair(box.id, box));
	}
}

void SceneManager::LoadMap(std::ifstream& file)
{
	std::string skipStr, filemap;
	int id = 0;
	file >> skipStr;
	while (skipStr != "$")
	{
		id = std::stoi(skipStr);
		file >> filemap >> skipStr;
		std::string path = Globals::scenePath + filemap;
		std::ifstream fFile(path);
		if (!fFile.is_open())
		{
			std::cerr << "Error opening file " << path << "\n";
			return;
		}
		MapInfo map = LoadElementsMap(fFile);
		map.id = id;
		m_mapList.push_back(map);
	}
}

MapInfo SceneManager::LoadElementsMap(std::ifstream& file)
{
	MapInfo map;
	std::string skipStr;
	file >> skipStr >> map.idTexture >> skipStr >> map.sizeByTile.x >> map.sizeByTile.y >> map.minTileSize >> map.maxTileSize;
	std::string typeStr;
	GLint type;
	do
	{
		if (!(file >> typeStr))
		{
			break;
		}
		type = ParseElementType(typeStr);
		switch (type)
		{
		case MT_PLANES:
			LoadPlanesMap(file, map);
			break;
		case MT_ENEMIES:
			LoadEnemiesMap(file, map);
			break;
		case MT_ITEMS:
			LoadItemsMap(file, map);
			break;
		default:
			std::cerr << "Map file format error, abort loading elements";
			file.close();
			return map;
		}
	} while (true);
	file.close();
	return map;
}

void SceneManager::LoadPlanesMap(std::ifstream& file, MapInfo& map)
{
	std::string skipStr;
	float x, y, z, w;
	file >> skipStr;
	while (skipStr != "$")
	{
		x = std::stoi(skipStr);
		file >> y >> z >> w >> skipStr;
		map.plane.push_back(Vector4(x, y, z, w));
	}
}

void SceneManager::LoadEnemiesMap(std::ifstream& file, MapInfo& map)
{
	std::string skipStr;
	int x, y, z;
	file >> skipStr;
	while (skipStr != "$")
	{
		x = std::stoi(skipStr);
		file >> y >> z >> skipStr;
		map.enemies.push_back(Vector3(x, y, z));
	}
}

void SceneManager::LoadItemsMap(std::ifstream& file, MapInfo& map)
{
	std::string skipStr;
	int x, y, z;
	file >> skipStr >> skipStr >> skipStr >> skipStr;
	while (skipStr != "$")
	{
		x = std::stoi(skipStr);
		file >> y >> z >> skipStr;
		map.items.push_back(Vector3(x, y, z));
	}
}

int ParseElementType(const std::string& type)
{
	if (type == "#Objects")
		return ET_OBJECT;
	if (type == "#Cameras")
		return ET_CAMERA;
	if (type == "#Enemies")
		return ET_ENEMY;
	if (type == "#Bullets")
		return ET_BULLET;
	if (type == "#Maps")
		return ET_MAP;
	if (type == "#PlanesMap")
		return MT_PLANES;
	if (type == "#EnemiesMap")
		return MT_ENEMIES;
	if (type == "#ItemsMap")
		return MT_ITEMS;
	return ET_INVALID;
}
