#include "stdafx.h"
#include "SpriteAnimation.h"
#include "Vertex.h"
#include "ResourcesManager.h"

SpriteAnimation::SpriteAnimation(GLint id, std::shared_ptr<Model> model, std::shared_ptr<Shaders> shader, std::shared_ptr<Texture> texture, GLint numAction, GLint numFrame, GLfloat timeBtwFrame) : BaseObject(id, model, shader, texture)
{
	m_currentAction = m_currentFrame = 0;
	m_numActions = numAction;
	m_numFrames = numFrame;
	m_timeBtwFrame = timeBtwFrame;
	m_currFrameTime = timeBtwFrame;
	m_changed = true;
}

SpriteAnimation::SpriteAnimation(const char* filename, GLint numAction, GLint numFrame, GLfloat timeBtwFrame) :
	BaseObject(0,
		ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT),
		ResourcesManager::GetInstance()->GetShader(1),
		ResourcesManager::GetInstance()->GetTexture(filename))
{
	m_currentAction = m_currentFrame = 0;
	m_numActions = numAction;
	m_numFrames = numFrame;
	m_timeBtwFrame = timeBtwFrame;
	m_currFrameTime = timeBtwFrame;
	m_changed = true;
}

void SpriteAnimation::SetAction(GLint action)
{
	m_currentAction = action;
}

void SpriteAnimation::Draw()
{
	glUseProgram(m_shader->program);
	glBindBuffer(GL_ARRAY_BUFFER, m_model->GetVBOid());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_model->GetIBOid());
	glBindTexture(GL_TEXTURE_2D, m_texture->GetTextureId());

	// update world matrix
	if (m_changed || m_camera->m_changed)
	{
		CalculateWVPMatrix();
		m_changed = false;
	}

	if (m_shader->positionAttribute != -1)
	{
		glEnableVertexAttribArray(m_shader->positionAttribute);
		glVertexAttribPointer(m_shader->positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}

	if (m_shader->uvAttribute != -1)
	{
		glEnableVertexAttribArray(m_shader->uvAttribute);
		glVertexAttribPointer(m_shader->uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) * 4));
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

	if (m_shader->uniformNumActions != -1)
	{
		glUniform1f(m_shader->uniformNumActions, (GLfloat)m_numActions);
	}

	if (m_shader->uniformNumFrames != -1)
	{
		glUniform1f(m_shader->uniformNumFrames, (GLfloat)m_numFrames);
	}

	if (m_shader->uniformCurrAction != -1)
	{
		glUniform1f(m_shader->uniformCurrAction, (GLfloat)m_currentAction);
	}

	if (m_shader->uniformCurrFrame != -1)
	{
		glUniform1f(m_shader->uniformCurrFrame, (GLfloat)m_currentFrame);
	}

	glDrawElements(GL_TRIANGLES, m_model->GetNumIndices(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SpriteAnimation::Update(float deltaTime)
{
	// change frame
	if (m_currFrameTime <= 0)
	{
		m_currFrameTime = m_timeBtwFrame;
		m_currentFrame++;
		m_currentFrame = m_currentFrame >= m_numFrames ? 0 : m_currentFrame;
	}
	m_currFrameTime -= deltaTime;
}

void SpriteAnimation::SetNumFrame(GLint numFrame)
{
	m_numFrames = numFrame;
}

void SpriteAnimation::SetCurrentFrame(GLint currentFrame)
{
	m_currentFrame = currentFrame;
}

void SpriteAnimation::SetTimeBtwFrame(GLfloat time)
{
	m_timeBtwFrame = time;
}

GLfloat SpriteAnimation::TimeAction()
{
	return m_timeBtwFrame * m_numFrames;
}

bool SpriteAnimation::IsLastFrame()
{
	return m_currentFrame == m_numFrames - 1 ? true : false;
}

Vector2 SpriteAnimation::GetSize()
{
	return Vector2(m_scale.x, m_scale.y);
}

GLint SpriteAnimation::GetCurrentFrame()
{
	return m_currentFrame;
}
