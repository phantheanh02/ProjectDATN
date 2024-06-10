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
	button->Set2DSize(220, 70);
	button->Set2DPosition(710, 30);
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
	auto sprite2D = std::make_shared<Sprite2D>("state_background.png");
	sprite2D->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	sprite2D->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_sprite2DList.push_back(sprite2D);

	// info table
	sprite2D = std::make_shared<Sprite2D>("BorderAttribute.png");
	sprite2D->Set2DSize(360, 450);
	sprite2D->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	sprite2D->Set2DPosition(570, 150);
	m_sprite2DList.push_back(sprite2D);

	// stats
	auto pos = Vector2(730, 260);
	auto stats = SceneManager::GetInstance()->GetCharacterStats(currentCharacter);
	sprite2D = std::make_shared<Sprite2D>(stats.hp + 83);
	sprite2D->Set2DSize(168, 20);
	sprite2D->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	sprite2D->Set2DPosition(pos.x, pos.y);
	m_sprite2DList.push_back(sprite2D);

	sprite2D = std::make_shared<Sprite2D>(stats.spd + 89);
	sprite2D->Set2DSize(168, 20);
	sprite2D->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	sprite2D->Set2DPosition(pos.x, pos.y + 67);
	m_sprite2DList.push_back(sprite2D);

	sprite2D = std::make_shared<Sprite2D>(stats.atk + 95);
	sprite2D->Set2DSize(168, 20);
	sprite2D->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	sprite2D->Set2DPosition(pos.x, pos.y + 134);
	m_sprite2DList.push_back(sprite2D);

	// Bullet type
	auto id = (currentCharacter - 29) / 6;
	m_bulletAnimation = std::make_shared<SpriteAnimation>(72 + id, 4, 0.1);
	m_bulletAnimation->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_bulletAnimation->Set2DPosition(750, 520);
	m_bulletAnimation->Set2DSize(96, 96);
}

void GSChooseCharacter::Update(float deltaTime)
{
	m_bulletAnimation->Update(deltaTime);
	m_currentCharacter->Update(deltaTime);
}

void GSChooseCharacter::Draw()
{
	for (auto it : m_sprite2DList)
	{
		it->Draw();
	}

	for (auto button : m_buttonList)
	{
		button->Draw();
	}
	m_bulletAnimation->Draw();
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
			GLint id = 0;
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
			auto stats = SceneManager::GetInstance()->GetCharacterStats((CharacterType)id);
			if (stats.type)
			{
				m_sprite2DList[2]->SetTexture(ResourcesManager::GetInstance()->GetTexture(stats.hp + 83));
				m_sprite2DList[3]->SetTexture(ResourcesManager::GetInstance()->GetTexture(stats.spd + 89));
				m_sprite2DList[4]->SetTexture(ResourcesManager::GetInstance()->GetTexture(stats.atk + 95));
				m_bulletAnimation->SetTexture(ResourcesManager::GetInstance()->GetTexture(72 + (id - 29) / 6));
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
