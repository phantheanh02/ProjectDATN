#pragma once
#include <GLES2/gl2.h>
#include <memory>
#include "../Utilities/Math.h"
#include "Globals.h"

enum PlanetType
{
	MT_BAREN = 0,
	MT_LAVA,
	MT_ICE,
	MT_BLACK_HOLE,
	MT_TERRAN
};

class MapClass
{
public:
	MapClass(PlanetType type);
	~MapClass();

	void Init(GLint idTexture, GLint minTileSize, GLint maxTileSize, Vector2 sizeByTile);
	void Update(GLfloat deltaTime);
	void Draw();

	inline void SetPlaneList(std::vector<Vector4> list) { m_planeList = list; };
	inline void SetEnemiesList(std::vector<Vector3> list) { m_enemiesList = list; };
	inline void SetItemList(std::vector<Vector3> list) { m_itemsList = list; };

	inline PlanetType				GetType() { return m_type; };
	inline GLint					GetIdTexture() { return m_idTexture; };
	inline GLint					GetMinTileSize() { return m_minTileSize; };
	inline GLint					GetMaxTileSize() { return m_maxTileSize; };
	inline Vector2					GetSizeByTile() { return m_sizeByTile; };
	inline std::vector<Vector4>		GetPlaneList() { return m_planeList; };
	inline std::vector<Vector3>		GetEnemiesList() { return m_enemiesList; };
	inline std::vector<Vector3>		GetItemList() { return m_itemsList; };
	
private:
	PlanetType	m_type;
	GLint		m_idTexture;
	GLint		m_minTileSize;
	GLint		m_maxTileSize;
	Vector2		m_sizeByTile;

	std::vector<Vector4>	m_planeList;		// x, y: position; z, w : size
	std::vector<Vector3>	m_enemiesList;	// x: id texture; y, z: position
	std::vector<Vector3>	m_itemsList;		// x" id texture; y, z: position

};