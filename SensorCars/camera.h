#pragma once

#include "position.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace gfx
{
	class Camera :public mth::Position
	{
		mth::matrix m_projectionMatrix;
		int m_mx, m_my;
		bool m_isADown;
		bool m_isWDown;
		bool m_isSDown;
		bool m_isDDown;
		bool m_isCrltDown;
		bool m_isSpaceDown;
		bool m_isShiftDown;

	public:
		Camera();
		void setProjection(float screenAspect = 16.0f / 9.0f);
		mth::matrix getProjectionMatrix();
		mth::matrix getViewMatrix();
		mth::matrix getCameraMatrix();

		void MessageHandler(MSG& msg);
		void Update(float deltaTime);
	};
}