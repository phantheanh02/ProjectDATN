#include "stdafx.h"
#include "GSChooseCharacter.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "Globals.h"

extern CharacterType currentCharacter;

GSChooseCharacter::~GSChooseCharacter()
{
}

void GSChooseCharacter::Init()
{
	// chacracter
	m_currentCharacter = std::make_shared<SpriteAnimation>(currentCharacter, 5, 0.1);
	m_currentCharacter->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_currentCharacter->Set2DPosition(300, 350);
	m_currentCharacter->Set2DSize(300, 300);

	// back button
	auto button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	button->Set2DSize(175, 55);
	button->Set2DPosition(750, 30);
	m_buttonList.push_back(button);

	// Left arrow
	button = std::make_shared<Button>("Icons/icon_arrow.png", BUTTON_NEXT_LEFT);
	button->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::L_RETANGLE_CENTER));
	button->Set2DSize(90, 70);
	button->Set2DPosition(100, 350);
	m_buttonList.push_back(button);

	// Right arrow
	button = std::make_shared<Button>("Icons/icon_arrow.png", BUTTON_NEXT_RIGHT);
	button->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));
	button->Set2DSize(90, 70);
	button->Set2DPosition(500, 350);
	m_buttonList.push_back(button);
	// bg
	m_background = std::make_shared<Sprite2D>("state_background.png");
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
}

void GSChooseCharacter::Update(float deltaTime)
{
	m_background->Update(deltaTime);
	m_currentCharacter->Update(deltaTime);
}

void GSChooseCharacter::Draw()
{
	m_background->Draw();
	for (auto button : m_buttonList)
	{
		button->Draw();
	}
	m_currentCharacter->Draw();
}

void GSChooseCharacter::Pause()
{
}

void GSChooseCharacter::Resume()
{
}

void GSChooseCharacter::Exit()
{
}

void GSChooseCharacter::HandleEvent()
{
}

void GSChooseCharacter::OnKey(unsigned char key, bool pressed)
{
}

void GSChooseCharacter::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	std::cout << x << " " << y << "\n";
	for (auto& button : m_buttonList)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			GLint id;
			switch (button->m_type)
			{
			case BUTTON_BACK:
				currentCharacter = (CharacterType)((m_currentCharacter->GetTexture()->GetID()));
				ResourcesManager::GetInstance()->GetSound(1)->Play();
				GameStateMachine::GetInstance()->PopState();
				break;
			case BUTTON_NEXT_LEFT:
				id = m_currentCharacter->GetTexture()->GetID();
				id = id - 6 < 29 ? 53 : id - 6;
				m_currentCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(id));
				break;
			case BUTTON_NEXT_RIGHT:
				id = m_currentCharacter->GetTexture()->GetID();
				id = id + 6 > 53 ? 29 : id + 6;
				m_currentCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(id));
				std::cout << id << "\n";
				break;
			default:
				break;
			}
		};
	}
}

void GSChooseCharacter::OnMouseMove(int x, int y)
{
}

void GSChooseCharacter::OnMouseScroll(int x, int y, short delta)
{
}
