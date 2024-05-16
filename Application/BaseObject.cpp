#include "stdafx.h"

#include "BaseObject.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Model.h"
#include "Shaders.h"
#include "Texture.h"
#include "ResourcesManager.h"

BaseObject::BaseObject()
{
	m_alpha = 1.0f;
	m_id = -1;
	m_changed = true;
	m_camera = SceneManager::GetInstance()->GetCamera(CameraType::DYNAMIC_CAMERA);
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
	m_camera = SceneManager::GetInstance()->GetCamera(CameraType::DYNAMIC_CAMERA);
}

BaseObject::~BaseObject()
{
	m_model.reset();
	m_shader.reset();
	m_texture.reset();
	m_camera.reset();
}

bool BaseObject::HasTouchMouse(GLfloat x, GLfloat y, bool bIsPressed)
{
	if ((m_pos.x <= x) && (x <= m_pos.x + m_scale.x)
		&& (m_pos.y <= y) && (y <= m_pos.y + m_scale.y)
		&& bIsPressed)
	{
		return true;
	}
	return false;
}

void BaseObject::Set2DSize(GLint x, GLint y)
{
	auto scale = Vector3(x, y, 1.0f);
	SetScale(scale);
	m_transAfterFlip.y = m_flippedX ? (float)y : 0;
	m_transAfterFlip.x = m_flippedY ? (float)x : 0;
	m_changed = true;
}

void BaseObject::Set2DSizeByTile(GLfloat width, GLfloat height)
{
	width *= tileSizeByPixel;
	height *= tileSizeByPixel;

	auto scale = Vector3(width, height, 1.0f);
	SetScale(scale);
	m_transAfterFlip.y = m_flippedX ? height : 0;
	m_transAfterFlip.x = m_flippedY ? width : 0;
	m_changed = true;
}

void BaseObject::Set2DPosition(GLint x, GLint y)
{
	auto pos = Vector3(x, y, 0.0f);
	m_pos = m_translation = pos;
	m_changed = true;
}

void BaseObject::Set2DPositionByTile(GLfloat x, GLfloat y)
{
	x *= tileSizeByPixel;
	y *= tileSizeByPixel;

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
	ModelType type = m_model->GetType();
	switch (type)
	{
	case R_RETANGLE_TOPRIGHT:
		m_model = ResourcesManager::GetInstance()->GetModel(ModelType::L_RETANGLE_TOPRIGHT);
		break;
	case R_RETANGLE_CENTER:
		m_model = ResourcesManager::GetInstance()->GetModel(ModelType::L_RETANGLE_CENTER);
		break;
	case L_RETANGLE_TOPRIGHT:
		m_model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
		break;
	case L_RETANGLE_CENTER:
		m_model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER);
		break;
	default:
		break;
	}
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
