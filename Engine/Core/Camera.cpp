#include "pch.h"
#include "Camera.h"

namespace soku {
	Camera::Camera()
	{
	}
	Matrix Camera::GetViewRow() const
	{
		return 
			Matrix::CreateTranslation(-m_position) *
			Matrix::CreateRotationY(-m_yaw)*
			Matrix::CreateRotationX(-m_pitch);
	}
	Matrix Camera::GetCubeViewRow() const
	{
		return
			Matrix::CreateRotationY(-m_yaw) *
			Matrix::CreateRotationX(-m_pitch);
	}
	Matrix Camera::GetProjRow() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(m_projFovAngleY),
			m_aspect, m_nearZ, m_farZ);
	}
	Vector3 Camera::GetEyePos() const
	{
		return m_position;
	}
	void Camera::UpdateMouse(int deltaX, int deltaY)
	{
		delPitch = DirectX::XMConvertToRadians(deltaX * m_rotationSpeed);
		delYaw = DirectX::XMConvertToRadians(deltaY * m_rotationSpeed);
		//std::cout << delPitch << ' ' << delYaw << '\n';
 		m_yaw += delPitch;
		m_pitch += delYaw;
		if (m_pitch >= DirectX::XM_PI / 2.f) {
			m_pitch = DirectX::XM_PI / 2.f;
		}
		else if(m_pitch <= -DirectX::XM_PI / 2.f)
		{
			m_pitch = -DirectX::XM_PI / 2.f;
		}
		Matrix m = Matrix::CreateRotationX(m_pitch) *
			Matrix::CreateRotationY(m_yaw);
		m_viewDir = Vector3::Transform(Vector3(0.f,0.f,1.f), m);
		m_viewDir.Normalize();
		m_rightDir = m_upDir.Cross(m_viewDir);
		m_rightDir.Normalize();
	}
	
	void Camera::MoveForward(float dt)
	{
		m_position += m_viewDir * dt * m_speed;
	}
	void Camera::MoveRight(float dt)
	{
		m_position += m_rightDir * dt * m_speed;
	}
	void Camera::SetAspectRatio(float aspect)
	{
		m_aspect = aspect;
	}
}