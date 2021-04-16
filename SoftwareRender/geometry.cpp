#include "geometry.h"

Vec3f cross(Vec3f a, Vec3f b)
{
	return Vec3f(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}
