#pragma once

#include <memory>
#include "BaseObject.h"

class Sprite2D : public BaseObject
{
public:
	using BaseObject::BaseObject;
	Sprite2D(const char* filename);
	Sprite2D(GLint textureID);
	~Sprite2D();

	void Draw() override;
	void Update(GLfloat deltaTime) override;
	inline Vector2 Get2DSize() { return Vector2(m_scale.x, m_scale.y); };
	inline Vector2 Get2DPosition() { return Vector2(m_pos.x, m_pos.y); };
private:

};