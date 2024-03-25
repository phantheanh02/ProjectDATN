#pragma once
#include "../Utilities/Math.h"
#include <vector>
#include <string>

class Model
{
public:
	Model();
	~Model();
	GLint	LoadModel(const std::string& filename);

	GLuint	GetVBOid() { return m_vboId; };
	GLuint	GetIBOid() { return m_iboId; };
	GLuint	GetNumIndices() { return m_numIndices; };
	Vector3 GetPivot() const { return m_pivot; };
	GLint	GetID() { return m_id; };

	void	SetID(GLint id) { m_id = id; }


private:
	GLuint	m_vboId;
	GLuint	m_iboId;
	GLuint	m_numIndices;
	Vector3 m_pivot;
	GLint	m_id;
};