#include <GLES2/gl2.h>
#include <memory>
#include "Sprite2D.h"
#include "box2d.h"
#include "Globals.h"


class Item
{
public:
	Item(b2World* world ,GLint idTexture, GLfloat posX, GLfloat posY);
	~Item();

	void Update(GLfloat deltaTime);
	void Draw();

	void Set2DSize(GLint w, GLint h);

	inline bool IsLoot() { return m_isLoot; };
	inline void	Loot() { m_isLoot = true; };
	GLint	m_typeItem;
	GLint	m_tileSizeByPixel;
private:
	std::shared_ptr<Sprite2D> m_sprite;
	b2Body* m_body;
	Vector2 m_position;
	bool	m_isLoot;
};