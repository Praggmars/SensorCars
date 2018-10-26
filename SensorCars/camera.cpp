#include "camera.h"

namespace gfx
{
	Camera::Camera()
	{
		m_mx = 0;
		m_my = 0;
		m_isADown = false;
		m_isSDown = false;
		m_isWDown = false;
		m_isDDown = false;
		m_isCrltDown = false;
		m_isSpaceDown = false;
		m_isShiftDown = false;
	}
	void Camera::setProjection(float screenAspect)
	{
		m_projectionMatrix = mth::matrix::PerspectiveFOV(mth::pi / 4.0f, screenAspect, 0.1f, 1000.0f);
	}
	mth::matrix Camera::getProjectionMatrix()
	{
		return m_projectionMatrix;
	}
	mth::matrix Camera::getViewMatrix()
	{
		mth::float3 up(0.0f, 1.0f, 0.0f), lookAt(0.0f, 0.0f, 1.0f);
		mth::float4x4 rotationMatrix;

		rotationMatrix = mth::float4x4::Rotation(rotation);

		lookAt = rotationMatrix * lookAt;
		up = rotationMatrix * up;

		lookAt += position;

		return mth::float4x4::LookAt(position, lookAt, up);
	}
	mth::matrix Camera::getCameraMatrix()
	{
		return m_projectionMatrix * getViewMatrix();
	}
	void Camera::MessageHandler(MSG& msg)
	{
		int x, y;
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.wParam)
			{
			case 'A':
				m_isADown = true;
				break;
			case 'S':
				m_isSDown = true;
				break;
			case 'W':
				m_isWDown = true;
				break;
			case 'D':
				m_isDDown = true;
				break;
			case VK_CONTROL:
				m_isCrltDown = true;
				break;
			case VK_SPACE:
				m_isSpaceDown = true;
				break;
			case VK_SHIFT:
				m_isShiftDown = true;
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.wParam)
			{
			case 'A':
				m_isADown = false;
				break;
			case 'S':
				m_isSDown = false;
				break;
			case 'W':
				m_isWDown = false;
				break;
			case 'D':
				m_isDDown = false;
				break;
			case VK_CONTROL:
				m_isCrltDown = false;
				break;
			case VK_SPACE:
				m_isSpaceDown = false;
				break;
			case VK_SHIFT:
				m_isShiftDown = false;
				break;
			}
			break;
		case WM_MOUSEMOVE:
			x = LOWORD(msg.lParam);
			y = HIWORD(msg.lParam);
			if (msg.wParam & MK_LBUTTON)
			{
				LookUp((m_my - y) * 0.008f);
				TurnRight((x - m_mx)*0.008f);
			}
			m_mx = x;
			m_my = y;
			break;
		}
	}
	void Camera::Update(float deltaTime)
	{
		float delta = deltaTime * 2.0f;
		float forward = 0.0f;
		float right = 0.0f;
		float upward = 0.0f;
		if (m_isShiftDown)
			delta *= 5.0f;
		if (m_isWDown)
			forward += delta;
		if (m_isSDown)
			forward -= delta;
		if (m_isDDown)
			right += delta;
		if (m_isADown)
			right -= delta;
		if (m_isCrltDown)
			upward -= delta;
		if (m_isSpaceDown)
			upward += delta;
		MoveForward(forward);
		MoveRight(right);
		MoveUp(upward);
	}
}