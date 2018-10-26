#include "geometry.h"

namespace mth
{

	Triangle::Triangle(float3 tri[])
	{
		Triangle(tri[0], tri[1], tri[2]);
	}
	Triangle::Triangle(float3 tri1, float3 tri2, float3 tri3)
	{
		vertices[0] = tri1;
		vertices[1] = tri2;
		vertices[2] = tri3;
		normal = (vertices[1] - vertices[2]).Cross(vertices[1] - vertices[0]).Normalized();
		distance = normal.Dot(vertices[0]);
	}

	bool isPointOverTriangle(float3 tri[], float3 point)
	{
		float3 vecs[3] = { tri[1] - tri[0],
			tri[2] - tri[0],
			point - tri[0] };
		float dot00 = vecs[0].Dot(vecs[0]);
		float dot01 = vecs[0].Dot(vecs[1]);
		float dot02 = vecs[0].Dot(vecs[2]);
		float dot11 = vecs[1].Dot(vecs[1]);
		float dot12 = vecs[1].Dot(vecs[2]);
		float denom = dot00 * dot11 - dot01 * dot01;
		float u = (dot11 * dot02 - dot01 * dot12) / denom;
		float v = (dot00 * dot12 - dot01 * dot02) / denom;
		return u >= 0.0f && v >= 0.0f && u + v <= 1.0f;
	}
	float DirectionalDistanceFromTriangle(Position ray, Triangle tri)
	{
		return DirectionalDistanceFromTriangle(ray.position, float3x3::Rotation(ray.rotation)*float3(0.0f, 0.0f, 1.0f), tri);
	}
	float DirectionalDistanceFromTriangle(float3 raypos, float3 raydir, Triangle tri)
	{
		float distfromtriplain = tri.normal.Dot(raypos) - tri.distance;
		float disttowardplain = -tri.normal.Dot(raydir);
		if (disttowardplain < 0.0f)
			return NAN;
		float dist = distfromtriplain / disttowardplain;
		float3 point = dist * raydir + raypos;
		return isPointOverTriangle(tri.vertices, point) ? dist : NAN;
	}
}