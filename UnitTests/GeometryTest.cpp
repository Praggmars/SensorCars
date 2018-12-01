#include "CppUnitTest.h"
#include "../SensorCars/geometry.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(GeometryTest)
	{
	public:

		TEST_METHOD(PointOnTriangleTest)
		{
			mth::float3 tri[] = {
				mth::float3(0.0f, 0.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f),
				mth::float3(1.0f, 0.0f, 0.0f)
			};
			mth::float3 p1(0.25f, 0.0f, 0.25f);
			Assert::IsTrue(mth::isPointOverTriangle(tri, p1));
			mth::float3 p2(0.75f, 0.0f, 0.5f);
			Assert::IsFalse(mth::isPointOverTriangle(tri, p2));
		}

		TEST_METHOD(DistanceFromTriangleTest)
		{
			mth::Triangle triangle(
				mth::float3(0.0f, 0.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f),
				mth::float3(1.0f, 0.0f, 0.0f));
			mth::Position p;
			p.position = mth::float3(0.25f, 1.0f, 0.25f);
			p.rotation = mth::float3(mth::pi / 2.0f, 0.0f, 0.0f);
			Assert::IsTrue(mth::DirectionalDistanceFromTriangle(p, triangle) == 1.0f);
		}

	};
}