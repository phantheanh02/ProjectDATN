#include "stdafx.h"
#include "Camera.h"
#include "Globals.h"

Camera::Camera()
{
}

Camera::Camera(GLint id)
{
	m_id = id;
	m_up = Vector3(0, 1, 0);
	m_pos = Vector3(0.0f, 0.0f, 0.0f);
	m_target = Vector3(0.0f, 0.0f, -1.0f);
	m_fMoveSpeed = 0.5f;
	m_fRotateSpeed = 0.5f;
	m_fNear = -1.0f;
	m_fFar = 1.0f;
	m_fov = 1.0f;
	m_projectionMatrix.SetOrthographic(m_fNear, m_fFar, 0.0f, Globals::screenWidth, 0.0f, Globals::screenHeight);
	CalculateWorldMatrix();
	CalculateViewMatrix();
}

Camera::Camera(GLint id, Vector3 pos, GLfloat fnear, GLfloat ffar, GLfloat fov, GLfloat rspeed, GLfloat mspeed) : m_id(id), m_fov(fov), m_pos(pos), m_fNear(fnear), m_fFar(ffar), m_fMoveSpeed(mspeed), m_fRotateSpeed(rspeed)
{
	m_target = Vector3(0.0f, 0.0f, -1.0f);
	//m_projectionMatrix.SetPerspective(m_fov, (GLfloat)(Globals::screenWidth) / Globals::screenHeight, m_fNear, m_fFar);
	m_projectionMatrix.SetOrthographic(-1.0f, 1.0f, 0.0f, Globals::screenWidth, 0.0f, Globals::screenHeight);
	//m_projectionMatrix.SetOrthographic(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
	CalculateWorldMatrix();
	CalculateViewMatrix();
}

Camera::~Camera()
{
}

void Camera::SetTarget(Vector3 target)
{
	m_target = target;
	m_changed = true;
}

void Camera::SetPosition(Vector3 pos)
{
	m_pos = pos;
	m_changed = true;
}

void Camera::SetId(GLint id)
{
	m_id = id;
}

Matrix Camera::GetWorldMatrix()
{

	if (m_changed)
	{
		CalculateWorldMatrix();
		m_changed = false;
	}
	return m_worldMatrix;
}

Matrix Camera::GetViewMatrix()
{
	if (m_changed)
	{
		CalculateViewMatrix();
		m_changed = false;
	}
	return m_viewMatrix;
}

void Camera::CalculateWorldMatrix()
{
	Matrix R, T;
	T.SetIdentity();
	T.m[3][0] = m_pos.x;	T.m[3][1] = m_pos.y;	T.m[3][2] = m_pos.z;
	Vector3 zaxis = (m_pos - m_target).Normalize();
	Vector3 xaxis = m_up.Cross(zaxis).Normalize();
	Vector3 yaxis = (zaxis.Cross(xaxis).Normalize());
	R.SetIdentity();
	R.m[0][0] = xaxis.x;	R.m[0][1] = xaxis.y;	R.m[0][2] = xaxis.z;
	R.m[1][0] = yaxis.x;	R.m[1][1] = yaxis.y;	R.m[1][2] = yaxis.z;
	R.m[2][0] = zaxis.x;	R.m[2][1] = zaxis.y;	R.m[2][2] = zaxis.z;
	m_worldMatrix = R * T;
}

void Camera::CalculateViewMatrix()
{
	//Vector3 zaxis = (m_pos - m_target).Normalize();
	Vector3 zaxis = (Vector3(0.0f, 0.0f, 1.0f)).Normalize();
	Vector3 xaxis = m_up.Cross(zaxis).Normalize();
	Vector3 yaxis = (zaxis.Cross(xaxis).Normalize());
	m_viewMatrix.m[0][0] = xaxis.x;		m_viewMatrix.m[0][1] = yaxis.x;		m_viewMatrix.m[0][2] = zaxis.x;		m_viewMatrix.m[0][3] = 0.0f;
	m_viewMatrix.m[1][0] = xaxis.y;		m_viewMatrix.m[1][1] = yaxis.y;		m_viewMatrix.m[1][2] = zaxis.y;		m_viewMatrix.m[1][3] = 0.0f;
	m_viewMatrix.m[2][0] = xaxis.z;		m_viewMatrix.m[2][1] = yaxis.z;		m_viewMatrix.m[2][2] = zaxis.z;		m_viewMatrix.m[2][3] = 0.0f;
	m_viewMatrix.m[3][0] = -m_pos.Dot(xaxis);		m_viewMatrix.m[3][1] = -m_pos.Dot(yaxis);			m_viewMatrix.m[3][2] = -m_pos.Dot(zaxis);			m_viewMatrix.m[3][3] = 1.0f;
}

void Camera::Reset()
{
	m_up = Vector3(0, 1, 0);
	m_pos = Vector3(0.0f, 0.0f, 0.0f);
	m_target = Vector3(0.0f, 0.0f, -1.0f);
	m_fMoveSpeed = 0.5f;
	m_fRotateSpeed = 0.5f;
	m_fNear = -1.0f;
	m_fFar = 1.0f;
	m_fov = 1.0f;
	m_projectionMatrix.SetOrthographic(m_fNear, m_fFar, 0.0f, Globals::screenWidth, 0.0f, Globals::screenHeight);
	CalculateWorldMatrix();
	CalculateViewMatrix();
}
