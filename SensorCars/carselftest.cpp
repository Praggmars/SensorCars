#include "carselftest.h"

namespace car
{
	void SelfTest::ForwardDriveTest()
	{
		if (!(m_testProgress & (UINT)TestPartFlag::FORWARD_DRIVE))
		{
			m_lastPosition = m_testSubject.position;
			m_testSubject.setSpeed(m_speed);
			m_testProgress |= (UINT)TestPartFlag::FORWARD_DRIVE;
		}
		else
		{
			if (m_testSubject.getSpeed() != m_speed ||
				m_lastPosition == m_testSubject.position)
				m_testFailed |= (UINT)TestPartFlag::FORWARD_DRIVE;
			m_lastPosition = m_testSubject.position;
		}
	}
	void SelfTest::BackwardDriveTest()
	{
		if (!(m_testProgress & (UINT)TestPartFlag::BACKWARD_DRIVE))
		{
			m_lastPosition = m_testSubject.position;
			m_testSubject.setSpeed(-m_speed);
			m_testProgress |= (UINT)TestPartFlag::BACKWARD_DRIVE;
		}
		else
		{
			if (m_testSubject.getSpeed() != -m_speed ||
				m_lastPosition == m_testSubject.position)
				m_testFailed |= (UINT)TestPartFlag::BACKWARD_DRIVE;
			m_lastPosition = m_testSubject.position;
		}
	}
	void SelfTest::RightSteerTest()
	{
		if (!(m_testProgress & (UINT)TestPartFlag::RIGHT_STEER))
		{
			m_lastRotation = m_testSubject.rotation;
			m_testSubject.setSpeed(m_speed);
			m_testSubject.setSteering(-m_steering);
			m_testProgress |= (UINT)TestPartFlag::RIGHT_STEER;
		}
		else
		{
			if (m_testSubject.getSteering() != -m_steering ||
				m_lastRotation == m_testSubject.rotation)
				m_testFailed |= (UINT)TestPartFlag::RIGHT_STEER;
			m_lastRotation = m_testSubject.rotation;
		}
	}
	void SelfTest::LeftSteerTest()
	{
		if (!(m_testProgress & (UINT)TestPartFlag::LEFT_STEER))
		{
			m_lastRotation = m_testSubject.rotation;
			m_testSubject.setSpeed(-m_speed);
			m_testSubject.setSteering(m_steering);
			m_testProgress |= (UINT)TestPartFlag::LEFT_STEER;
		}
		else
		{
			if (m_testSubject.getSteering() != m_steering ||
				m_lastRotation == m_testSubject.rotation)
				m_testFailed |= (UINT)TestPartFlag::LEFT_STEER;
			m_lastRotation = m_testSubject.rotation;
		}
	}
	void SelfTest::LightSensorTest()
	{
		if (!(m_testProgress & (UINT)TestPartFlag::LIGHT_SENSOR))
		{
			m_testSubject.setSpeed(0.0f);
			m_testProgress |= (UINT)TestPartFlag::LIGHT_SENSOR;
		}
		for (int i = 0; i < m_testSubject.getLightSensorCount(); i++)
			if (!(m_testSubject.getLightSensor(i).getMeasurement() > 0.0f))
				m_testFailed |= (UINT)TestPartFlag::LIGHT_SENSOR;
	}
	void SelfTest::DistanceSensorTest()
	{
		if (!(m_testProgress & (UINT)TestPartFlag::DISTANCE_SENSOR))
		{
			m_testSubject.setSpeed(0.0f);
			m_testProgress |= (UINT)TestPartFlag::DISTANCE_SENSOR;
		}
		for (int i = 0; i < m_testSubject.getDistanceSensorCount(); i++)
			if (!(m_testSubject.getDistanceSensor(i).getMeasurement() > 0.0f))
				m_testFailed |= (UINT)TestPartFlag::DISTANCE_SENSOR;
	}
	SelfTest::SelfTest(Car& testSubject)
		:m_testSubject(testSubject),
		m_time(0.0f),
		m_testProgress(0),
		m_testFailed(0),
		m_lastPosition(),
		m_speed(1.0f),
		m_steering(0.5f),
		m_finished() {}

	void SelfTest::Update(float deltaTime)
	{
		if (!m_finished)
		{
			float timeStep = 0.5f;
			m_time += deltaTime;
			if (m_time < 1.0f*timeStep)
				ForwardDriveTest();
			else if (m_time < 2.0f*timeStep)
				BackwardDriveTest();
			else if (m_time < 3.0f*timeStep)
				RightSteerTest();
			else if (m_time < 4.0f*timeStep)
				LeftSteerTest();
			else if (m_time < 5.0f*timeStep)
				LightSensorTest();
			else if (m_time < 6.0f*timeStep)
				DistanceSensorTest();
			else
				m_finished = true;
		}
	}
}
