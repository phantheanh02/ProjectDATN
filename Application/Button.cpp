#include "stdafx.h"
#include "Button.h"
#include "SceneManager.h"

Button::Button(std::shared_ptr<Model> model, std::shared_ptr<Shaders> shader, std::shared_ptr<Texture> texture, ButtonType type) :
	Sprite2D(0, model, shader, texture), m_type(type)
{
	m_camera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);
}

Button::Button(const char* filename, ButtonType type) :
	Sprite2D(filename), m_type(type)
{
	m_camera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);
}

Button::Button(GLint idTexture, ButtonType type) :
	Sprite2D(idTexture), m_type(type)
{
	m_camera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);
}

Button::Button()
{

}

Button::~Button()
{

}


bool Button::HandleTouchMouse(GLfloat x, GLfloat y, bool bIsPressed)
{
	if (bIsPressed)
	{
		if (m_model->GetType() == ModelType::L_RETANGLE_TOPRIGHT || m_model->GetType() == ModelType::R_RETANGLE_TOPRIGHT)
		{
			if ((m_pos.x <= x) && (x <= m_pos.x + m_fWidth)
				&& (m_pos.y <= y) && (y <= m_pos.y + m_fHeight))
			{
				m_alpha = 0.5f;
				return true;
			}
		}
		else
		{
			if ((m_pos.x - m_fWidth / 2 <= x) && (x <= m_pos.x + m_fWidth / 2)
				&& (m_pos.y - m_fHeight / 2 <= y) && (y <= m_pos.y + m_fHeight / 2))
			{
				m_alpha = 0.5f;
				return true;
			}
		}
	}
	

	m_alpha = 1.0f;

	return false;
}

void Button::HandleMoveMouse(GLfloat x, GLfloat y)
{
	if (m_model->GetType() == ModelType::L_RETANGLE_TOPRIGHT || m_model->GetType() == ModelType::R_RETANGLE_TOPRIGHT)
	{
		if ((m_pos.x <= x) && (x <= m_pos.x + m_fWidth)
			&& (m_pos.y <= y) && (y <= m_pos.y + m_fHeight))
		{
			m_alpha = 0.5f;
			return;
		}
	}
	else
	{
		if ((m_pos.x - m_fWidth / 2 <= x) && (x <= m_pos.x + m_fWidth / 2)
			&& (m_pos.y - m_fHeight / 2 <= y) && (y <= m_pos.y + m_fHeight / 2))
		{
			m_alpha = 0.5f;
			return;
		}
	}

	m_alpha = 1.0f;
}

void Button::Set2DSize(GLint x, GLint y)
{
	m_fWidth = x;
	m_fHeight = y;

	auto scale = Vector3(x, y, 1.0f);
	SetScale(scale);
	m_changed = true;
}

