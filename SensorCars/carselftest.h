#pragma once

#include "car.h"

enum class TestPartFlag :UINT
{
	FORWARD_DRIVE = 1 << 0,
	BACKWARD_DRIVE = 1 << 1,
	RIGHT_STEER = 1 << 2,
	LEFT_STEER = 1 << 3,
	LIGHT_SENSOR = 1 << 4,
	DISTANCE_SENSOR = 1 << 5
};

namespace car
{
	class SelfTest
	{
		Car& m_testSubject;
		float m_time;
		UINT m_testProgress;
		UINT m_currentTest;
		UINT m_testFailed;
		mth::float3 m_lastPosition;
		mth::float3 m_lastRotation;
		float m_speed;
		float m_steering;
		bool m_finished;

	private:
		void ForwardDriveTest();
		void BackwardDriveTest();
		void RightSteerTest();
		void LeftSteerTest();
		void LightSensorTest();
		void DistanceSensorTest();

	public:
		SelfTest(Car& testSubject);

		void Update(float deltaTime);
		inline UINT TestResult() { return m_testFailed; }
		inline bool Finished() { return m_finished; }
	};
}