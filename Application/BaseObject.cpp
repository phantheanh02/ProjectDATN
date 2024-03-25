#include "stdafx.h"
#include "BaseObject.h"
#include "SceneManager.h"

BaseObject::BaseObject()
{
	m_alpha = 1.0f;
	m_id = -1;
	m_changed = true;
	m_camera = SceneManager::GetInstance()->GetCamera(0);
}

BaseObject::BaseObject(GLint id, std::shared_ptr<Model> model, std::shared_ptr<Shaders> shaders, std::shared_ptr<Texture> texture)
{
	m_alpha = 1.0f;
	m_id = id;
	m_model = model;
	m_shader = shaders;
	m_texture = texture;
	m_changed = true;
	m_flippedX = false;
	m_flippedY = false;
	m_camera = SceneManager::GetInstance()->GetCamera(0);
}

BaseObject::~BaseObject()
{
	m_model.reset();
	m_shader.reset();
	m_texture.reset();
	m_camera.reset();
}

void BaseObject::Set2DSize(GLint x, GLint y)
{
	auto scale = Vector3(x, y, 1.0f);
	SetScale(scale);
	m_transAfterFlip.y = m_flippedX ? (float)y : 0;
	m_transAfterFlip.x = m_flippedY ? (float)x : 0;
	m_changed = true;
}

void BaseObject::Set2DPosition(GLint x, GLint y)
{
	auto pos = Vector3(x, y, 0.0f);
	m_pos = m_translation = pos;
	m_changed = true;
}

void BaseObject::Set2DScale(GLfloat x, GLfloat y)
{
	SetScale(Vector3(x * m_scale.x, y * m_scale.y, 1.0f));
	m_changed = true;
}

void BaseObject::SetTexture(std::shared_ptr<Texture> texture)
{
	m_texture = texture;
}

void BaseObject::FlipHorizontal()
{
	if (!m_flippedX)
	{
		m_flip.x += 3.14159f;
		m_flippedX = true;
	}
	else
	{
		m_flip.x = 0.0f;
		m_flippedX = false;
	}
	m_transAfterFlip.y = m_flippedX ? m_scale.y : 0;
	m_changed = true;
}

void BaseObject::FlipVertical()
{
	if (!m_flippedY)
	{
		m_flip.y += 3.14159f;
		m_flippedY = true;
	}
	else
	{
		m_flip.y = 0.0f;
		m_flippedY = false;
	}
	m_transAfterFlip.x = m_flippedY ? m_scale.x : 0;
	m_changed = true;
}

void BaseObject::CalculateWVPMatrix()
{
	Matrix S, R, Rx, Ry, Rz, T, V, P;
	S.SetScale(m_scale);
	Rx.SetRotationX(m_rotation.x + m_flip.x);
	Ry.SetRotationY(m_rotation.y + m_flip.y);
	Rz.SetRotationZ(m_rotation.z);
	R = Rz * Rx * Ry;
	T.SetTranslation(m_translation + m_transAfterFlip);
	V = m_camera->GetViewMatrix();
	P = m_camera->GetProjectionMatrix();
	m_wvpMatrix = S * R * T * V * P;
}
