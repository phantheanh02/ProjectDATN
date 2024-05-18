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

	bool IsLastFrame();

	void SetNumFrame(GLint numFrame);
	void SetCurrentFrame(GLint currentFrame);
	void SetTimeBtwFrame(GLfloat time);

	inline GLfloat	GetTimeAction() { return m_defAnimation.timeBtwFrame * m_defAnimation.numFrames; };
	inline Vector2	GetSize() { return Vector2(m_scale.x, m_scale.y); };
	inline GLint	GetCurrentFrame() { return m_defAnimation.currentFrame; };
	inline Vector2	GetPosition() { return Vector2(m_pos.x, m_pos.y); };
	inline GLint	GetNumFrames() { return m_defAnimation.numFrames; };
private:
	DefAnimation	m_defAnimation;
};