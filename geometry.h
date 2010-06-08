#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "cg.h"
#include "cgl.h"
#include "basic_types.h"

inline int max(int a, int b)
{
	return a > b ? a : b;
}
inline int min(int a, int b)
{
	return a < b ? a : b;
}
inline int sgn(double a)
{
	return a < 0 ? -1 : a == 0 ? 0 : 1;
}

void ship_to_tile(const struct ship*, struct tile*);
int tiles_intersect(const struct tile*, const struct tile*, struct rect*);

#endif
