#pragma once
#include "Sprite2D.h"

#define HP_ENEMY_MAX 3

struct DefAnimation
{
	// need get from RM file
	GLint		idTexture;
	GLint		numFrames;
	GLfloat		timeBtwFrame;

	// not need
	GLint		numActions;
	GLint		currentFrame;
	GLint		currentAction;
	GLfloat		currFrameTime;

	DefAnimation();
	DefAnimation(GLint id, GLint numFrame, GLfloat time) : idTexture(id), numFrames(numFrame), timeBtwFrame(time)
	{
		numActions = 1;
		currentFrame = 0;
		currentAction = 0;
		currFrameTime = 0;
	}
};

class SpriteAnimation : public BaseObject
{
public:
	SpriteAnimation(GLint id, std::shared_ptr<Model> model, std::shared_ptr<Shaders> shader, std::shared_ptr<Texture> texture, GLint numFrame, GLfloat timeBtwFrame);
	SpriteAnimation(const char* filename, GLint numFrame, GLfloat timeBtwFrame);
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
	DefAnimation	m_defAnimation;
};