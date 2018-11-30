#include "stdafx.h"
#include "CppUnitTest.h"
#include "../SensorCars/linalg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(Multiply)
		{
			Assert::IsTrue(
				mth::float2x2(
					1, 2,
					3, 4)*
				mth::float2x2(
					2, 0,
					1, 2) ==
				mth::float2x2(
					4, 4,
					10, 8));

			Assert::IsTrue(
				mth::float3x3(
					1, 2, 3,
					4, 5, 6,
					7, 8, 9)*
				mth::float3x3(
					9, 8, 7,
					6, 5, 4,
					3, 2, 1) ==
				mth::float3x3(
					30, 24, 18,
					84, 69, 54,
					138, 114, 90));

			Assert::IsTrue(
				mth::float4x4(
					5, 2, 6, 1,
					0, 6, 2, 0,
					3, 8, 1, 4,
					1, 8, 5, 6) *
				mth::float4x4(
					7, 5, 8, 0,
					1, 8, 2, 6,
					9, 4, 3, 8,
					5, 3, 7, 9) ==
				mth::float4x4(
					96, 68, 69, 69,
					24, 56, 18, 52,
					58, 95, 71, 92,
					90, 107, 81, 142));
		}

		TEST_METHOD(Determinant)
		{
			Assert::IsTrue(
				mth::float2x2(
					8, 15,
					7, -3)
				.Determinant() == -129);

			Assert::IsTrue(
				mth::float3x3(
					4, -3, 0,
					2, -1, 2,
					1, 5, 7)
				.Determinant() == -32);

			Assert::IsTrue(
				mth::float4x4(
					3, 0, 2, -1,
					1, 2, 0, -2,
					4, 0, 6, -3,
					5, 0, 2, 0)
				.Determinant() == 20);
		}

		TEST_METHOD(Inverse)
		{
			float eps = 1e-6;

			mth::float2x2 m22(
				1.0f, 2.0f,
				-1.25f, 0.375f);
			Assert::IsTrue(mth::float2x2::Identity().isNear(m22*m22.Inverse(), eps));

			mth::float3x3 m33(
				1.0f, 2.0f, -1.25f,
				0.375f, -0.5f, 0.5f,
				0.75f, 1.0f, -0.75f);
			Assert::IsTrue(mth::float3x3::Identity().isNear(m33*m33.Inverse(), eps));

			mth::float4x4 m44(
				1.0f, 2.0f, -1.25f, 0.375f,
				-0.5f, 0.5f, 0.75f, 1.0f,
				-0.75f, -1.875f, -0.125f, 0.0f,
				0.25f, 0.0625f, 1.0f, -0.375f);
			Assert::IsTrue(mth::float4x4::Identity().isNear(m44*m44.Inverse(), eps));
		}

	};
}