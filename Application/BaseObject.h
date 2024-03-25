#pragma once
#include <memory>
#include "../Utilities/Math.h"
#include "Model.h"
#include "Shaders.h"
#include "Texture.h"
#include "Camera.h"

class BaseObject
{
public:
	BaseObject();
	BaseObject(GLint id, std::shared_ptr<Model> model, std::shared_ptr<Shaders> shaders, std::shared_ptr<Texture> texture);
	~BaseObject();

	virtual void Draw() = 0;
	virtual void Update(GLfloat deltaTime) = 0;

	inline void AttachCamera(std::shared_ptr<Camera> camera) { m_camera = camera; };
	inline void SetRotation(Vector3 rotation) { m_rotation = rotation; };
	inline void SetScale(Vector3 scale) { m_scale = scale; };
	inline void SetPos(Vector3 pos) { m_pos = pos; };
	inline void SetTransparency(GLfloat alpha) { m_alpha = alpha; };
	void Set2DSize(GLint x, GLint y);
	void Set2DPosition(GLint x, GLint y);
	void Set2DScale(GLfloat x, GLfloat y);
	void SetTexture(std::shared_ptr<Texture> texture);
	void FlipHorizontal();
	void FlipVertical();
	void CalculateWVPMatrix();

protected:
	std::shared_ptr<Model> m_model;
	std::shared_ptr<Shaders> m_shader;
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<Camera> m_camera;

	GLint m_id;
	Vector3 m_pos;
	GLfloat m_alpha;
	Vector3 m_scale = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 m_rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_translation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_flip = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 m_transAfterFlip = Vector3(0.0f, 0.0f, 0.0f);
	Matrix m_wvpMatrix;
	bool m_changed, m_flippedX, m_flippedY;
};
