#pragma once
#include "../Utilities/Math.h"
#include <vector>
#include <string>

enum ModelType
{
	// R : draw left to right
	// L:  draw right to left
	R_RETANGLE_TOPRIGHT,
	R_RETANGLE_CENTER,
	L_RETANGLE_TOPRIGHT,
	L_RETANGLE_CENTER,
	TOP_RETANGLE_CENTER,
	BOTTOM_RETANGLE_CENTER
};

class Model
{
public:
	Model();
	~Model();
	GLint	LoadModel(const std::string& filename);

	inline GLuint		GetVBOid() { return m_vboId; };
	inline GLuint		GetIBOid() { return m_iboId; };
	inline GLuint		GetNumIndices() { return m_numIndices; };
	inline Vector3		GetPivot() const { return m_pivot; };
	inline GLint		GetID() { return m_id; };
	ModelType			GetType();

	inline void			SetID(GLint id) { m_id = id; }


private:
	GLuint		m_vboId;
	GLuint		m_iboId;
	GLuint		m_numIndices;
	Vector3		m_pivot;
	GLint		m_id;
};