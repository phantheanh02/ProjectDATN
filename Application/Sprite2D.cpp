#include "stdafx.h"
#include "Sprite2D.h"
#include "Vertex.h"
#include "Globals.h"
#include "ResourcesManager.h"
#include "BaseObject.h"

Sprite2D::Sprite2D(const char* filename) : 
	BaseObject(0, 
		ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT), 
		ResourcesManager::GetInstance()->GetShader(0), 
		ResourcesManager::GetInstance()->GetTexture(filename))
{
}

Sprite2D::Sprite2D(GLint textureID):
	BaseObject(0,
		ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT),
		ResourcesManager::GetInstance()->GetShader(0),
		ResourcesManager::GetInstance()->GetTexture(textureID))
{

}

Sprite2D::~Sprite2D()
{
}

void Sprite2D::Draw()
{
	glUseProgram(m_shader->program);
	glBindBuffer(GL_ARRAY_BUFFER, m_model->GetVBOid());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_model->GetIBOid());
	glBindTexture(GL_TEXTURE_2D, m_texture->GetTextureId());

	if (m_camera->m_changed || m_changed)
	{
		CalculateWVPMatrix();
		m_changed = false;
	}

	if (m_shader->positionAttribute != -1)
	{
		glVertexAttribPointer(m_shader->positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(m_shader->positionAttribute);
	}

	if (m_shader->uvAttribute != -1)
	{
		glVertexAttribPointer(m_shader->uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) * 4));
		glEnableVertexAttribArray(m_shader->uvAttribute);
	}

	if (m_shader->uniformWVP != -1)
	{
		glUniformMatrix4fv(m_shader->uniformWVP, 1, GL_FALSE, m_wvpMatrix.m[0]);
	}

	if (m_shader->iTextureLoc != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_shader->iTextureLoc, 0);
	}

	if (m_shader->uniformAlpha != -1)
	{
		glUniform1f(m_shader->uniformAlpha, m_alpha);
	}

	glDrawElements(GL_TRIANGLES, m_model->GetNumIndices(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Sprite2D::Update(GLfloat deltaTime)
{

}
