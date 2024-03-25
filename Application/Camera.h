#pragma once
#include "../Utilities/Math.h"

class Camera
{
public:
	Camera();
	Camera(GLint id);
	Camera(GLint id, Vector3 pos, GLfloat fnear, GLfloat ffar, GLfloat fov, GLfloat rspeed, GLfloat mspeed);
	~Camera();

	void SetTarget(Vector3 target);
	void SetPosition(Vector3 pos);
	void SetId(GLint id);

	Matrix	GetWorldMatrix();
	Matrix	GetViewMatrix();
	Matrix	GetProjectionMatrix() const { return m_projectionMatrix; };
	Vector3 GetPosition() const { return m_pos; };
	GLfloat GetNear() const { return m_fNear; };
	GLfloat GetFar() const { return m_fFar; };
	GLfloat GetRotateSpeed() const { return m_fRotateSpeed; };
	GLfloat GetMoveSpeed() const { return m_fMoveSpeed; };
	GLint	GetId() const { return m_id; };

	void CalculateWorldMatrix();
	void CalculateViewMatrix();

	GLboolean m_changed = true;
private:
	GLint m_id;

	Vector3 m_pos;
	Vector3 m_target;
	Vector3 m_up;

	Matrix m_worldMatrix;
	Matrix m_viewMatrix;
	Matrix m_projectionMatrix;

	GLfloat m_fMoveSpeed;
	GLfloat m_fRotateSpeed;

	GLfloat m_fNear;
	GLfloat m_fFar;
	GLfloat m_fov;

};