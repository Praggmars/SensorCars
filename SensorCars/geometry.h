#pragma once

#include "position.h"

namespace mth
{
	struct Triangle
	{
		mth::float3 vertices[3];
		mth::float3 normal;
		float distance;

		Triangle() = default;
		Triangle(float3 tri[]);
		Triangle(float3 tri1, float3 tri2, float3 tri3);
	};

	bool isPointOverTriangle(float3 tri[], float3 point);

	float DirectionalDistanceFromTriangle(Position ray, Triangle tri);
	float DirectionalDistanceFromTriangle(float3 raypos, float3 raydir, Triangle tri);
}