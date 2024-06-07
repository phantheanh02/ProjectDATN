#include "stdafx.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "GSSetting.h"

GSSetting::~GSSetting()
{
}

void GSSetting::Init()
{
	auto RM = ResourcesManager::GetInstance();
	auto SM = SceneManager::GetInstance();

	m_volumeChanged = false;
	m_currentVolume = SM->m_soundVolume;
	m_lastNonZeroVolume = SM->m_lastNonZeroSoundVolume;

	auto staticCamera = SM->GetCamera(CameraType::STATIC_CAMERA);

	m_background = std::make_shared<Sprite2D>("state_background.png");
	m_background->Set2DPosition(-10, -10);
	m_background->Set2DSize(1200, 900);
	m_background->AttachCamera(staticCamera);

	// get font
	auto font = RM->GetFont(0);

	// set text
	auto textSettingHeader = std::make_shared<Text>(0);
	textSettingHeader->AttachCamera(staticCamera);
	textSettingHeader->Init(font, "SETTING");
	textSettingHeader->Set2DPosition(297, 55);
	textSettingHeader->Set2DScale(1.0f, 1.0f);
	textSettingHeader->SetTextColor(255, 255, 0);

	auto textSound = std::make_shared<Text>(0);
	textSound->AttachCamera(staticCamera);
	textSound->Init(font, "Sound");
	textSound->Set2DPosition(222, 249);
	textSound->Set2DScale(0.75f, 0.75f);
	textSound->SetTextColor(255, 255, 0);

	auto textVolume = std::make_shared<Text>(0);
	textVolume->AttachCamera(staticCamera);
	textVolume->Init(font, "Volume");
	textVolume->Set2DPosition(222, 347);
	textVolume->Set2DScale(0.75f, 0.75f);
	textVolume->SetTextColor(255, 255, 0);

	m_textVolumeLevel = std::make_shared<Text>(0);
	m_textVolumeLevel->AttachCamera(staticCamera);
	m_textVolumeLevel->Init(font, std::to_string(m_currentVolume));
	m_textVolumeLevel->Set2DPosition(590, 350);
	m_textVolumeLevel->Set2DScale(0.75f, 0.75f);
	m_textVolumeLevel->SetTextColor(255, 255, 0);

	// push text to list
	m_listText.push_back(textSettingHeader);
	m_listText.push_back(textSound);
	m_listText.push_back(textVolume);
	m_listText.push_back(m_textVolumeLevel);

	// init buttons
	auto button = std::make_shared<Button>("btn_save.png", BUTTON_SAVE);
	button->Set2DPosition(200, 530);
	button->Set2DSize(220, 70);
	m_listButton.push_back(button);

	button = std::make_shared<Button>("btn_cancel.png", BUTTON_CANCEL);
	button->Set2DPosition(540, 530);
	button->Set2DSize(220, 70);
	m_listButton.push_back(button);

	button = std::make_shared<Button>("Icons/icon_back.png", BUTTON_BACK);
	button->Set2DPosition(480, 350);
	button->Set2DSize(60, 60);
	m_listButton.push_back(button);

	button = std::make_shared<Button>("Icons/icon_next.png", BUTTON_NEXT);
	button->Set2DPosition(730, 350);
	button->Set2DSize(60, 60);
	m_listButton.push_back(button);

	if (m_currentVolume <= 0)
	{
		m_buttonSoundToggle = std::make_shared<Button>("Icons/icon_sound_off.png", BUTTON_SOUND);
	}
	else
	{
		m_buttonSoundToggle = std::make_shared<Button>("Icons/icon_sound_on.png", BUTTON_SOUND);
	}
	m_buttonSoundToggle->Set2DPosition(570, 250);
	m_buttonSoundToggle->Set2DSize(50, 50);

}

void GSSetting::Update(float deltaTime)
{
	if (m_currentVolume == 0)
	{
		m_buttonSoundToggle->SetTexture(ResourcesManager::GetInstance()->GetTexture("Icons/icon_sound_off.png"));
	}
	else
	{
		m_buttonSoundToggle->SetTexture(ResourcesManager::GetInstance()->GetTexture("Icons/icon_sound_on.png"));
	}
	if (m_volumeChanged)
	{
		m_textVolumeLevel->SetText(std::to_string(m_currentVolume));
		m_volumeChanged = false;
	}
	ResourcesManager::GetInstance()->SetMasterVolume(m_currentVolume);
}

void GSSetting::Draw()
{
	m_background->Draw();
	m_textVolumeLevel->CalculateWVPMatrix();

	for (auto& text : m_listText)
	{
		text->Draw();
	}

	for (auto& btn : m_listButton)
	{
		btn->Draw();
	}
}

void GSSetting::Pause()
{
}

void GSSetting::Resume()
{
}

void GSSetting::Exit()
{
}

void GSSetting::HandleEvent()
{
}

void GSSetting::OnKey(unsigned char key, bool pressed)
{
}

void GSSetting::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	for (auto& button : m_listButton)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_SAVE:
				SceneManager::GetInstance()->m_soundVolume = m_currentVolume;
				SceneManager::GetInstance()->m_lastNonZeroSoundVolume = m_lastNonZeroVolume;
				GameStateMachine::GetInstance()->PopState();
				break;
			case BUTTON_CANCEL:
				GameStateMachine::GetInstance()->PopState();
				break;
			case BUTTON_BACK:
				m_lastNonZeroVolume = m_currentVolume != 0 ? m_currentVolume : m_lastNonZeroVolume;
				m_currentVolume -= 10;
				m_currentVolume = m_currentVolume <= 0 ? 0 : m_currentVolume;
				m_volumeChanged = true;
				break;
			case BUTTON_NEXT:
				m_currentVolume += 10;
				m_currentVolume = m_currentVolume >= 100 ? 100 : m_currentVolume;
				m_volumeChanged = true;
				break;
			case BUTTON_SOUND:
				if (m_currentVolume == 0)
				{
					// click when volume == 0 - toggle from mute
					m_currentVolume = m_lastNonZeroVolume;
					m_volumeChanged = true;
				}
				else
				{
					// click when volume != 0 - mute
					m_lastNonZeroVolume = m_currentVolume;
					m_currentVolume = 0;
					m_volumeChanged = true;
				}
				break;
			default:
				break;
			}
		};
	}
}

void GSSetting::OnMouseMove(int x, int y)
{
	for (auto& button : m_listButton)
	{
		button->HandleMoveMouse(x, y);
	}
}

void GSSetting::OnMouseScroll(int x, int y, short delta)
{
}
