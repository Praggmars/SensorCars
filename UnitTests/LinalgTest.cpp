#include "CppUnitTest.h"
#include "../SensorCars/linalg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(LinalgTest)
	{
	public:

		TEST_METHOD(NormalTest)
		{
			float eps = 1e-6f;
			Assert::IsTrue(mth::float2(3.0f, 4.0f).Normalized().isNear(mth::float2(0.6f, 0.8f), eps));
			Assert::IsTrue(mth::float3(2.0f, 3.0f, 4.0f).Normalized().isNear(mth::float3(
				0.37139067635f, 0.55708601453f, 0.7427813527f), eps));
			Assert::IsTrue(mth::float4(1.0f, 2.0f, 3.0f, 4.0f).Normalized().isNear(mth::float4(
				0.18257418583f, 0.36514837167f, 0.5477225575f, 0.73029674334f), eps));
		}

		TEST_METHOD(Transposed)
		{
			Assert::IsTrue(
				mth::float2x2(
					1, 2,
					3, 4).Transposed() ==
				mth::float2x2(
					1, 3,
					2, 4));

			Assert::IsTrue(
				mth::float3x3(
					1, 2, 3,
					4, 5, 6,
					7, 8, 9).Transposed() ==
				mth::float3x3(
					1, 4, 7,
					2, 5, 8,
					3, 6, 9));

			Assert::IsTrue(
				mth::float4x4(
					1, 2, 3, 4,
					3, 4, 5, 6,
					5, 6, 7, 8,
					1, 9, 4, 2).Transposed() ==
				mth::float4x4(
					1, 3, 5, 1,
					2, 4, 6, 9,
					3, 5, 7, 4,
					4, 6, 8, 2));
		}

		TEST_METHOD(Multiply_VecMat)
		{
			Assert::IsTrue(
				mth::float2(5, 6)*
				mth::float2x2(
					1, 3,
					2, 4) ==
				mth::float2(17, 39));

			Assert::IsTrue(
				mth::float3(2, 4, 7)*
				mth::float3x3(
					1, 4, 7,
					2, 5, 8,
					3, 6, 9) ==
				mth::float3(31, 70, 109));

			Assert::IsTrue(
				mth::float4(2, 4, 2, 1)*
				mth::float4x4(
					1, 3, 5, 1,
					2, 4, 6, 9,
					3, 5, 7, 4,
					4, 6, 8, 2) ==
				mth::float4(20, 38, 56, 48));
		}

		TEST_METHOD(Multiply_MatVec)
		{
			Assert::IsTrue(
				mth::float2x2(
					1, 2,
					3, 4)*
				mth::float2(5, 6) ==
				mth::float2(17, 39));

			Assert::IsTrue(
				mth::float3x3(
					1, 2, 3,
					4, 5, 6,
					7, 8, 9)*
				mth::float3(2, 4, 7) ==
				mth::float3(31, 70, 109));

			Assert::IsTrue(
				mth::float4x4(
					1, 2, 3, 4,
					3, 4, 5, 6,
					5, 6, 7, 8,
					1, 9, 4, 2)*
				mth::float4(2, 4, 2, 1) ==
				mth::float4(20, 38, 56, 48));
		}

		TEST_METHOD(Multiply_mm)
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
			float eps = 1e-6f;

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