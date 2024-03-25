#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include "Model.h"
#include "Vertex.h"


Model::Model()
{

}

Model::~Model()
{
	glDeleteBuffers(1, &m_vboId);
	glDeleteBuffers(1, &m_vboId);
}

GLint Model::LoadModel(const std::string& filename)
{
	FILE* fmodel;
	fopen_s(&fmodel, filename.c_str(), "r");
	if (!fmodel)
	{
		std::cerr << "Couldn't open file: " << filename << "\n";
		return -1;
	}
	//std::cout << "Open " << filename << "\n";
	Vector3 tempPivot = Vector3(0, 0, 0);
	GLint numVertices;
	Vertex tempVertex;
	std::vector<Vertex> vertexData;

	// get number of vertices
	fscanf_s(fmodel, "%*s %d", &numVertices);

	// get vertices
	for (int i = 0; i < numVertices; i++)
	{
		fscanf_s(fmodel, "%*d. pos:[%f, %f, %f]; norm:[%f, %f, %f]; binorm:[%f, %f, %f]; tgt:[%f, %f, %f]; uv:[%f, %f];",
			&tempVertex.pos.x, &tempVertex.pos.y, &tempVertex.pos.z,
			&tempVertex.norm.x, &tempVertex.norm.y, &tempVertex.norm.z,
			&tempVertex.binorm.x, &tempVertex.binorm.y, &tempVertex.binorm.z,
			&tempVertex.tgt.x, &tempVertex.tgt.y, &tempVertex.tgt.z,
			&tempVertex.uv.x, &tempVertex.uv.y);
		tempPivot.x += tempVertex.pos.x;
		tempPivot.y += tempVertex.pos.y;
		tempPivot.z += tempVertex.pos.z;
		vertexData.push_back(tempVertex);
	}
	m_pivot = tempPivot / (GLfloat)numVertices;
	glGenBuffers(1, &m_vboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// get number of indices
	fscanf_s(fmodel, "%*s %d", &m_numIndices);

	// get indices
	std::vector<GLuint> indices;
	GLuint i1, i2, i3;
	for (GLuint i = 0; i < m_numIndices / 3; i++)
	{
		fscanf_s(fmodel, "%*d.%d,%d,%d", &i1, &i2, &i3);
		indices.push_back(i1);
		indices.push_back(i2);
		indices.push_back(i3);
	}
	
	glGenBuffers(1, &m_iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	fclose(fmodel);

	return 0;
}