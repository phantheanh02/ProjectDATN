#pragma once
#include "Sprite2D.h"

#define HP_ENEMY_MAX 3
class SpriteAnimation : public BaseObject
{
public:
	SpriteAnimation(GLint id, std::shared_ptr<Model> model, std::shared_ptr<Shaders> shader, std::shared_ptr<Texture> texture, GLint numAction, GLint numFrame, GLfloat timeBtwFrame);
	SpriteAnimation(const char* filename, GLint numAction, GLint numFrame, GLfloat timeBtwFrame);
	void SetAction(GLint action);

	void Draw() override;
	void Update(float deltaTime) override;

	void SetNumFrame(GLint numFrame);
	void SetCurrentFrame(GLint currentFrame);
	void SetTimeBtwFrame(GLfloat time);
	GLfloat TimeAction();
	bool IsLastFrame();
	Vector2 GetSize();
	GLint	GetCurrentFrame();
	inline Vector2 GetPosition() { return Vector2(m_pos.x, m_pos.y); };
private:
	GLint m_numFrames;
	GLint m_numActions;
	GLint m_currentFrame;
	GLint m_currentAction;

	GLfloat m_timeBtwFrame;
	GLfloat m_currFrameTime;
};